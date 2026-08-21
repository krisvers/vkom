#include <SDL3/SDL.h>

#include <vkom/enums.hpp>
#include <vkom/platform.hpp>
#include <vkom/instance.hpp>
#include <vkom/surface.hpp>
#include <vkom/adapter.hpp>
#include <vkom/device.hpp>
#include <vkom/pipeline.hpp>
#include <vkom/swapchain.hpp>
#include <vkom/heap.hpp>
#include <vkom/buffer.hpp>
#include <vkom/texture.hpp>
#include <vkom/queue.hpp>
#include <vkom/cmdencoder.hpp>
#include <vkom/cmdbatch.hpp>

#include <cstdio>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <algorithm>

#include <vkom/internal/object.hpp>

#ifdef VKOM_PLATFORM_FAMILY_APPLE
#include <SDL3/SDL_metal.h>
#endif

struct AutoReleaseEntry {
    vkom::IInterface* object;
    bool errorOnReleaseNonZero;
};

struct AutoReleasePool {
    std::vector<AutoReleaseEntry> entries = {};

    ~AutoReleasePool() {
        for (size_t i = 0; i < entries.size(); i += 1) {
            AutoReleaseEntry entry = entries[entries.size() - i - 1];
            vkom::IInterface* object = entry.object;
            vkom::ICollected* collected = object->queryInterface<vkom::ICollected>();
            if (collected != nullptr) {
                uint32_t referenceCount = collected->release();
                if (referenceCount != 0 && entry.errorOnReleaseNonZero) {
                    std::printf("Object %p denoted to have error on release non-zero return: %u\n", object, referenceCount);
                }

                continue;
            }

            vkom::IDestructible* destructible = object->queryInterface<vkom::IDestructible>();
            if (destructible != nullptr) {
                destructible->destroy();
                continue;
            }

            vkom::IDiscardable* discardable = object->queryInterface<vkom::IDiscardable>();
            if (discardable != nullptr) {
                discardable->discard();
                continue;
            }
        }
    }

    void push(vkom::IInterface* object, bool errorOnReleaseNonZero = true) {
        AutoReleaseEntry entry = {};
        entry.object = object;
        entry.errorOnReleaseNonZero = errorOnReleaseNonZero;

        entries.push_back(entry);
    }
};

class CollectedWrapper : virtual public vkom::internal::CollectedByHeap {
public:
    void* queryInterface(vkom::IID const& iid) noexcept override {
        if (iid == IBase::iid()) {
            return static_cast<IBase*>(this);
        } else if (iid == ICollected::iid()) {
            return static_cast<ICollected*>(this);
        }

        return nullptr;
    }
};

class SDLCollected final : virtual public CollectedWrapper {
public:
    SDLCollected(SDL_InitFlags flags) {
        if (!SDL_Init(flags)) {
            throw std::runtime_error("SDL_Init failed");
        }
    }

    ~SDLCollected() {
        SDL_Quit();
    }
};

class SDLCollectedWindow final : virtual public CollectedWrapper {
private:
    SDL_Window* _window = nullptr;

    #ifdef VKOM_PLATFORM_FAMILY_APPLE
    SDL_MetalView _view = nullptr;
    #endif

public:
    SDLCollectedWindow(const char* title, uint32_t width, uint32_t height, SDL_WindowFlags flags) {
        _window = SDL_CreateWindow(title, static_cast<int>(width), static_cast<int>(height), flags);
        if (_window == nullptr) {
            throw std::runtime_error("SDL_CreateWindow failed");
        }

        #ifdef VKOM_PLATFORM_FAMILY_APPLE
        _view = SDL_Metal_CreateView(_window);
        if (_view == nullptr) {
            throw std::runtime_error("SDL_Metal_CreateView failed");
        }
        #endif
    }

    ~SDLCollectedWindow() {
        #ifdef VKOM_PLATFORM_FAMILY_APPLE
        SDL_Metal_DestroyView(_view);
        #endif

        SDL_DestroyWindow(_window);
    }

    SDL_Window* window() const noexcept {
        return _window;
    }

    void getSurfaceWSIInfo(vkom::SurfaceWSIInfo* info) {
        *info = {};
        const char* videoDriver = SDL_GetCurrentVideoDriver();

        #ifdef VKOM_PLATFORM_FAMILY_NT
        info->type = vkom::SurfaceWSIType::Win32;
        info->windowHandle = reinterpret_cast<uint64_t>(SDL_GetPointerProperty(SDL_GetWindowProperties(_window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
        #elif defined(VKOM_PLATFORM_FAMILY_APPLE)
        info->type = vkom::SurfaceWSIType::Metal;
        info->windowHandle = reinterpret_cast<uint64_t>(SDL_Metal_GetLayer(_view));
        #elif defined(VKOM_PLATFORM_FAMILY_UNIX)
        if (std::strcmp(videoDriver, "x11") == 0) {
            info->type = vkom::SurfaceSurfaceWSIType::Xlib;
            info->windowHandle = reinterpret_cast<uint64_t>(SDL_GetPointerProperty(SDL_GetWindowProperties(_window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, nullptr));
            info->displayHandle = reinterpret_cast<uint64_t>(SDL_GetPointerProperty(SDL_GetWindowProperties(_window), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr));
        } else if (std::strcmp(videoDriver, "wayland") == 0) {
            info->type = vkom::SurfaceSurfaceWSIType::Wayland;
            info->windowHandle = reinterpret_cast<uint64_t>(SDL_GetPointerProperty(SDL_GetWindowProperties(_window), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr));
            info->displayHandle = reinterpret_cast<uint64_t>(SDL_GetPointerProperty(SDL_GetWindowProperties(_window), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr));
        }
        #endif
    }
};

std::vector<uint32_t> loadFile(const char* path) {
    std::ifstream in(path, std::ios::binary | std::ios::ate);
    if (!in.good()) {
        return {};
    }

    size_t byteSize = in.tellg();
    if (byteSize % 4 != 0) {
        in.close();
        return {};
    }

    std::vector<uint32_t> code(byteSize / 4);
    in.seekg(0, std::ios::beg);

    if (!in.read(reinterpret_cast<char*>(&code[0]), byteSize)) {
        in.close();
        return {};
    }

    in.close();
    return code;
}

/* trying to track retains and releases on VulkanDevice
    + VulkanAdapter::createDevice   :
    + createSwapchain               : VulkanManualSwapchain::VulkanManualSwapchain
    + acquireSemaphore              : VulkanSemaphore::VulkanSemaphore
    + acquireFence                  : VulkanFence::VulkanFence
    + createHeap                    : VulkanHeap::VulkanHeap
    + acquireQueue                  : VulkanQueue::VulkanQueue
    + createShaderModule            : VulkanShaderModule::VulkanShaderModule
    + createDescriptorSetLayout     : VulkanDescriptorSetLayout::VulkanDescriptorSetLayout
    + createDescriptorPool          : VulkanDescriptorPool::VulkanDescriptorPool
    + Vulkan::
    + Vulkan::
 */

int main(int argc, char** argv) {
    AutoReleasePool pool = {};

    SDLCollected* sdl = new SDLCollected(SDL_INIT_VIDEO);
    pool.push(sdl);

    SDLCollectedWindow* window = new SDLCollectedWindow("vkom test", 1200, 900, SDL_WINDOW_RESIZABLE);
    pool.push(window);

    vkom::InstanceLoaderInfo loaderInfo = {};

    vkom::IInstance* instance;
    if (vkom::createInstance(true, &loaderInfo, &instance) != vkom::Result::Success) {
        return 1;
    }

    pool.push(instance, true);

    instance->setLogCallback([](vkom::IInstance* instance, void* userData, vkom::DebugMessageSeverityFlags severity, vkom::DebugMessageTypeFlags type, const char* message) {
        std::printf("[vkom]: %s\n", message);
    }, nullptr);

    vkom::SurfaceWSIInfo surfaceWSIInfo;
    window->getSurfaceWSIInfo(&surfaceWSIInfo);

    vkom::IWSIInstance* wsiInstance = instance->queryInterface<vkom::IWSIInstance>();

    vkom::ISurface* surface;
    if (wsiInstance->createSurface(&surfaceWSIInfo, &surface) != vkom::Result::Success) {
        return 2;
    }

    pool.push(surface);

    vkom::IAdapter* adapter = instance->enumerateAdapters(0);

    vkom::IDevice* device;
    if (adapter->createDevice(&device) != vkom::Result::Success) {
        return 3;
    }

    pool.push(device);

    vkom::SurfaceCapabilities surfaceCapabilities;
    adapter->querySurfaceCapabilities(surface, &surfaceCapabilities);

    vkom::SwapchainInfo swapchainInfo = {};
    swapchainInfo.backbufferCount = std::max(surfaceCapabilities.minBackbufferCount, std::min(2u, surfaceCapabilities.maxBackbufferCount));
    swapchainInfo.backbufferInfo.usage = vkom::TextureUsageFlags::TransferDestination;
    swapchainInfo.backbufferInfo.dimensions.extent = surfaceCapabilities.currentExtent;
    swapchainInfo.backbufferInfo.dimensions.subresource.layers = 1;
    swapchainInfo.backbufferInfo.dimensions.subresource.mips = 1;
    swapchainInfo.preTransform = vkom::SurfaceTransformFlags::Identity;
    swapchainInfo.compositeAlpha = vkom::CompositeAlphaFlags::Opaque;
    swapchainInfo.surfaceFormatBits = adapter->querySurfaceFormatBits(surface, vkom::Format::BGRA8UnsignedNormSRGB, vkom::ColorSpaceFlags::All);
    swapchainInfo.presentModeFlags = vkom::PresentModeFlags::All;

    vkom::IWSIDevice* wsiDevice = device->queryInterface<vkom::IWSIDevice>();

    vkom::ISwapchain* swapchain;
    if (wsiDevice->createSwapchain(surface, &swapchainInfo, &swapchain) != vkom::Result::Success) {
        return 4;
    }

    pool.push(swapchain);

    vkom::ITimelineSemaphore* submissionFinishedTimelineSemaphore;
    {
        vkom::ISemaphore* submissionFinishedSemaphore;
        if (device->acquireSemaphore(true, &submissionFinishedSemaphore) != vkom::Result::Success) {
            return 5;
        }

        pool.push(submissionFinishedSemaphore);

        submissionFinishedTimelineSemaphore = submissionFinishedSemaphore->queryInterface<vkom::ITimelineSemaphore>();
        if (submissionFinishedTimelineSemaphore == nullptr) {
            return 6;
        }
    }

    vkom::IFence* batchFinishedFence;
    if (device->acquireFence(true, &batchFinishedFence) != vkom::Result::Success) {
        return 7;
    }

    pool.push(batchFinishedFence);

    vkom::IHeap* heap;
    if (device->createHeap(vkom::BufferUsageFlags::TransferSource | vkom::BufferUsageFlags::TransferDestination | vkom::BufferUsageFlags::StorageBuffer | vkom::BufferUsageFlags::ShaderDeviceAddress, vkom::TextureUsageFlags::None, vkom::MemoryLocationFlags::GPU, &heap) != vkom::Result::Success) {
        return 8;
    }

    pool.push(heap);

    vkom::BufferInfo bufferInfo = {};
    bufferInfo.size = 1024;
    bufferInfo.location = vkom::MemoryLocationFlags::GPU;
    bufferInfo.usage = vkom::BufferUsageFlags::TransferSource | vkom::BufferUsageFlags::TransferDestination | vkom::BufferUsageFlags::StorageBuffer | vkom::BufferUsageFlags::ShaderDeviceAddress;

    vkom::IBuffer* buffer;
    if (heap->createBuffer(&bufferInfo, &buffer) != vkom::Result::Success) {
        return 9;
    }

    pool.push(buffer);

    vkom::BufferViewInfo bufferViewInfo = {};
    bufferViewInfo.format = vkom::Format::Undefined;
    bufferViewInfo.offset = 0;
    bufferViewInfo.range = bufferInfo.size;

    vkom::IBufferView* bufferView;
    if (buffer->createView(&bufferViewInfo, &bufferView) != vkom::Result::Success) {
        return 10;
    }

    pool.push(bufferView);

    vkom::IQueue* queue;
    if (device->acquireQueue(vkom::QUEUE_FAMILY_ANY, vkom::QueueFlags::Graphics | vkom::QueueFlags::Present, &queue) != vkom::Result::Success) {
        return 11;
    }

    pool.push(queue);

    std::vector<uint32_t> shaderCode = loadFile("shader.spv");
    if (shaderCode.empty()) {
        return 12;
    }

    vkom::ShaderModuleInfo shaderInfo = {};
    shaderInfo.length = shaderCode.size();
    shaderInfo.spirv = &shaderCode[0];

    vkom::IShaderModule* shader;
    if (device->createShaderModule(&shaderInfo, &shader) != vkom::Result::Success) {
        return 13;
    }

    pool.push(shader);

    vkom::DescriptorBindingInfo descriptorSetLayoutBindings[1] = {};
    descriptorSetLayoutBindings[0].binding = 0;
    descriptorSetLayoutBindings[0].flags = vkom::DescriptorFlags::StorageBuffer;
    descriptorSetLayoutBindings[0].count = 1;
    descriptorSetLayoutBindings[0].stages = vkom::ShaderStageFlags::Compute;

    vkom::DescriptorSetLayoutInfo descriptorSetLayoutInfo = {};
    descriptorSetLayoutInfo.bindingCount = 1;
    descriptorSetLayoutInfo.bindings = &descriptorSetLayoutBindings[0];

    vkom::IDescriptorSetLayout* descriptorSetLayout;
    if (device->createDescriptorSetLayout(&descriptorSetLayoutInfo, &descriptorSetLayout) != vkom::Result::Success) {
        return 14;
    }

    pool.push(descriptorSetLayout);

    vkom::DescriptorPoolDescriptorInfo descriptorPoolDescriptors[1] = {};
    descriptorPoolDescriptors[0].count = 1;
    descriptorPoolDescriptors[0].flags = vkom::DescriptorFlags::StorageBuffer;

    vkom::DescriptorPoolInfo descriptorPoolInfo = {};
    descriptorPoolInfo.maxDescriptorSets = 1;
    descriptorPoolInfo.descriptorCount = 1;
    descriptorPoolInfo.descriptors = &descriptorPoolDescriptors[0];

    vkom::IDescriptorPool* descriptorPool;
    if (device->createDescriptorPool(&descriptorPoolInfo, &descriptorPool) != vkom::Result::Success) {
        return 15;
    }

    pool.push(descriptorPool);

    vkom::IDescriptorSet* descriptorSet;
    if (descriptorPool->allocateDescriptorSets(descriptorSetLayout, 1, &descriptorSet) != vkom::Result::Success) {
        return 16;
    }

    pool.push(descriptorSet);

    vkom::PushConstantRange computePipelineLayoutPushConstantRanges[1] = {};
    computePipelineLayoutPushConstantRanges[0].stages = vkom::ShaderStageFlags::Compute;
    computePipelineLayoutPushConstantRanges[0].offset = 0;
    computePipelineLayoutPushConstantRanges[0].size = sizeof(uint64_t);

    vkom::PipelineLayoutInfo computePipelineLayoutInfo = {};
    computePipelineLayoutInfo.descriptorSetLayoutCount = 1;
    computePipelineLayoutInfo.descriptorSetLayouts = &descriptorSetLayout;
    computePipelineLayoutInfo.pushConstantRangeCount = 1;
    computePipelineLayoutInfo.pushConstantRanges = &computePipelineLayoutPushConstantRanges[0];

    vkom::IPipelineLayout* computePipelineLayout;
    if (device->createPipelineLayout(&computePipelineLayoutInfo, &computePipelineLayout) != vkom::Result::Success) {
        return 17;
    }

    pool.push(computePipelineLayout);

    vkom::ComputePipelineInfo computePipelineInfo = {};
    computePipelineInfo.shaderInfo.shader = shader;
    computePipelineInfo.shaderInfo.stage = vkom::ShaderStageFlags::Compute;
    computePipelineInfo.shaderInfo.entry = "main";

    vkom::IComputePipeline* computePipeline;
    if (device->createComputePipeline(&computePipelineInfo, nullptr, computePipelineLayout, &computePipeline) != vkom::Result::Success) {
        return 18;
    }

    pool.push(computePipeline);

    uint64_t pastFinishedValue = 0;
    uint64_t futureFinishedValue = 1;

    vkom::ICommandEncoder* encoder = nullptr;
    vkom::ICommandBatch* batch = nullptr;
    vkom::IPresentFence* previousPresentFence = nullptr;

    bool quit = false;
    while (!quit) {
        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent)) {
            switch (sdlEvent.type) {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
                default:
                    break;
            }
        }

        if (previousPresentFence != nullptr) {
            previousPresentFence->wait();
            previousPresentFence->release();
        }

        futureFinishedValue = pastFinishedValue + 2;

        uint32_t index;

        vkom::SemaphorePoint signal = {};
        signal.semaphore = submissionFinishedTimelineSemaphore;
        signal.value = pastFinishedValue + 1;

        /* TODO: actually use backbuffer and present */
        vkom::Result result = swapchain->acquireNextIndex(&signal, nullptr, &index);
        if (result != vkom::Result::Success) {
            adapter->querySurfaceCapabilities(surface, &surfaceCapabilities);
            swapchainInfo.backbufferInfo.dimensions.extent = surfaceCapabilities.currentExtent;

            swapchain->recreate(&swapchainInfo);
            std::printf("%d\n", result);

            continue;
        }

        vkom::IBackbuffer* backbuffer = swapchain->enumerateBackbuffers(index);

        /* wait for previous batch to be finished */
        batchFinishedFence->wait();
        batchFinishedFence->reset();

        if (batch != nullptr) {
            batch->discard();
            batch = nullptr;
        }

        if (encoder != nullptr) {
            encoder->release();
            encoder = nullptr;
        }

        vkom::IResourceView* descriptorWriteViews[1] = {};
        descriptorWriteViews[0] = bufferView;

        vkom::DescriptorWrite descriptorWrite = {};
        descriptorWrite.binding = 0;
        descriptorWrite.element = 0;
        descriptorWrite.count = 1;
        descriptorWrite.views = &descriptorWriteViews[0];

        descriptorSet->write(1, &descriptorWrite);

        if (queue->acquireCommandEncoder(&encoder) != vkom::Result::Success) {
            return 2;
        }

        vkom::BufferInfo bufferInfo;
        buffer->getInfo(&bufferInfo);

        vkom::ComputePassDescriptor computePassDescriptor = {};

        vkom::IDeviceAddressBuffer* deviceAddressBuffer = buffer->queryInterface<vkom::IDeviceAddressBuffer>();
        uint64_t bufferDeviceAddress = deviceAddressBuffer->deviceAddress();

        vkom::IComputePass* computePass = encoder->beginComputePass(&computePassDescriptor);
        computePass->bindPipeline(computePipeline);
        computePass->bindDescriptorSet(computePipelineLayout, 0, descriptorSet, 0, nullptr);
        computePass->pushConstants(computePipelineLayout, vkom::ShaderStageFlags::Compute, 0, sizeof(bufferDeviceAddress), &bufferDeviceAddress);
        computePass->dispatch(bufferInfo.size / 4, 1, 1);
        computePass->end();

        vkom::TextureTransition transitionBackbufferToTransferDestination = {};
        transitionBackbufferToTransferDestination.general.srcStage = vkom::PipelineStageFlags::TopOfPipe;
        transitionBackbufferToTransferDestination.general.dstStage = vkom::PipelineStageFlags::Transfer;
        transitionBackbufferToTransferDestination.transfer.oldFamily = queue->family();
        transitionBackbufferToTransferDestination.transfer.newFamily = queue->family();
        transitionBackbufferToTransferDestination.oldLayout = vkom::TextureLayout::Undefined;
        transitionBackbufferToTransferDestination.newLayout = vkom::TextureLayout::TransferDestination;
        transitionBackbufferToTransferDestination.aspectFlags = vkom::TextureAspectFlags::Color;
        transitionBackbufferToTransferDestination.subresourcePosition.layer = 0;
        transitionBackbufferToTransferDestination.subresourcePosition.mip = 0;
        transitionBackbufferToTransferDestination.subresourceDimensions.layers = 1;
        transitionBackbufferToTransferDestination.subresourceDimensions.mips = 1;

        encoder->transitionTexture(backbuffer, &transitionBackbufferToTransferDestination);

        vkom::ColorTextureClear clearBackbuffer = {};
        clearBackbuffer.layout = vkom::TextureLayout::TransferDestination;
        clearBackbuffer.color[0] = 1.0f;
        clearBackbuffer.color[1] = 0.0f;
        clearBackbuffer.color[2] = 1.0f;
        clearBackbuffer.color[3] = 1.0f;
        clearBackbuffer.subresourceOffset.layer = 0;
        clearBackbuffer.subresourceOffset.mip = 0;
        clearBackbuffer.subresourceRange.layers = 1;
        clearBackbuffer.subresourceRange.mips = 1;

        encoder->clearColorTexture(backbuffer->queryInterface<vkom::ITransferDestinationTexture>(), &clearBackbuffer);

        vkom::TextureTransition transitionBackbufferToPresentSourceAndMakeTransferWriteVisible = {};
        transitionBackbufferToPresentSourceAndMakeTransferWriteVisible.general.srcStage = vkom::PipelineStageFlags::TopOfPipe;
        transitionBackbufferToPresentSourceAndMakeTransferWriteVisible.general.dstStage = vkom::PipelineStageFlags::Transfer;
        transitionBackbufferToPresentSourceAndMakeTransferWriteVisible.general.srcAccess = vkom::ResourceAccessFlags::None;
        transitionBackbufferToPresentSourceAndMakeTransferWriteVisible.general.dstAccess = vkom::ResourceAccessFlags::TransferRead;
        transitionBackbufferToPresentSourceAndMakeTransferWriteVisible.transfer.oldFamily = queue->family();
        transitionBackbufferToPresentSourceAndMakeTransferWriteVisible.transfer.newFamily = queue->family();
        transitionBackbufferToPresentSourceAndMakeTransferWriteVisible.oldLayout = vkom::TextureLayout::TransferDestination;
        transitionBackbufferToPresentSourceAndMakeTransferWriteVisible.newLayout = vkom::TextureLayout::PresentSource;
        transitionBackbufferToPresentSourceAndMakeTransferWriteVisible.aspectFlags = vkom::TextureAspectFlags::Color;
        transitionBackbufferToPresentSourceAndMakeTransferWriteVisible.subresourcePosition.layer = 0;
        transitionBackbufferToPresentSourceAndMakeTransferWriteVisible.subresourcePosition.mip = 0;
        transitionBackbufferToPresentSourceAndMakeTransferWriteVisible.subresourceDimensions.layers = 1;
        transitionBackbufferToPresentSourceAndMakeTransferWriteVisible.subresourceDimensions.mips = 1;

        encoder->transitionTexture(backbuffer, &transitionBackbufferToPresentSourceAndMakeTransferWriteVisible);

        if (encoder->batch(&batch) != vkom::Result::Success) {
            return 19;
        }

        vkom::CommandBatchSubmitWaitInfo submitWaits[1] = {};
        submitWaits[0].point.semaphore = submissionFinishedTimelineSemaphore;
        submitWaits[0].point.value = pastFinishedValue + 1;
        submitWaits[0].stageFlags = vkom::PipelineStageFlags::Transfer;

        vkom::CommandBatchSubmitSignalInfo submitSignals[1] = {};
        submitSignals[0].point.semaphore = submissionFinishedTimelineSemaphore;
        submitSignals[0].point.value = futureFinishedValue;

        vkom::CommandBatchSubmitInfo submitInfo = {};
        submitInfo.waitCount = 1;
        submitInfo.waits = &submitWaits[0];
        submitInfo.signalCount = 1;
        submitInfo.signals = &submitSignals[0];
        submitInfo.signalFence = batchFinishedFence;

        if (batch->submit(&submitInfo) != vkom::Result::Success) {
            return 20;
        }

        vkom::SemaphorePoint presentWaits[1] = {};
        presentWaits[0].semaphore = submissionFinishedTimelineSemaphore;
        presentWaits[0].value = futureFinishedValue;

        vkom::PresentInfo presentInfo = {};
        presentInfo.waitCount = 1;
        presentInfo.waits = &presentWaits[0];

        result = swapchain->present(queue, backbuffer, &presentInfo, &previousPresentFence);
        if (result != vkom::Result::Success) {
            adapter->querySurfaceCapabilities(surface, &surfaceCapabilities);
            swapchainInfo.backbufferInfo.dimensions.extent = surfaceCapabilities.currentExtent;

            swapchain->recreate(&swapchainInfo);
            std::printf("%d\n", result);
        }

        pastFinishedValue = futureFinishedValue;
    }

    device->waitIdle();

    if (batch != nullptr) {
        batch->discard();
    }

    if (encoder != nullptr) {
        encoder->release();
    }

    return 0;
}
