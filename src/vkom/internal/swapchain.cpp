#include <vkom/internal/swapchain.hpp>

#include <vkom/internal/enums.hpp>

namespace vkom {

namespace internal {

VulkanManualSwapchain::VulkanManualSwapchain(bool inheritedHandle, IDevice* device, ISurface* surface, SwapchainInfo const& info, VulkanSwapchainData const& swapchainData) : _inheritedHandle(inheritedHandle), _device(device), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _surface(surface), _info(info), _swapchainData(swapchainData) {
    /* TODO: backbuffer creation */
    uint32_t backbufferCount;
    PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR = _device->loadDispatchSymbol<PFN_vkGetSwapchainImagesKHR>("vkGetSwapchainImagesKHR");
    if (vkGetSwapchainImagesKHR == nullptr) {
        throw std::runtime_error("vkGetSwapchainImagesKHR failed");
    }

    if (vkGetSwapchainImagesKHR(_swapchainData.deviceData.vkDevice, _swapchainData.vkSwapchain, &backbufferCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("vkGetSwapchainImagesKHR failed");
    }

    _backbufferImages.resize(backbufferCount);
    if (vkGetSwapchainImagesKHR(_swapchainData.deviceData.vkDevice, _swapchainData.vkSwapchain, &backbufferCount, &_backbufferImages[0]) != VK_SUCCESS) {
        throw std::runtime_error("vkGetSwapchainImagesKHR failed");
    }

    _surface->retain();
    _device->retain();
}

VulkanManualSwapchain::~VulkanManualSwapchain() {
    _device->waitIdle();
    ParentByVector::disownAll();
    _device->disown(IInterface::queryInterface<IChild>());

    PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR = _device->loadDispatchSymbol<PFN_vkDestroySwapchainKHR>("vkDestroySwapchainKHR");
    if (vkDestroySwapchainKHR != nullptr) {
        vkDestroySwapchainKHR(_swapchainData.deviceData.vkDevice, _swapchainData.vkSwapchain, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    _surface->release();
    _device->release();
}

/* ISwapchain */
void VulkanManualSwapchain::getInfo(SwapchainInfo* info) const noexcept {
    *info = _info;
}

ISurface* VulkanManualSwapchain::surface() const noexcept {
    return _surface;
}

IBackbuffer* VulkanManualSwapchain::enumerateBackbuffers(uint32_t id) const noexcept {
    return IParent::enumerateChildren<IBackbuffer>(id);
}

Result VulkanManualSwapchain::recreate(SwapchainInfo const* info) noexcept {
    /* TODO: more intelligent surface format selection */
    SurfaceFormat surfaceFormat;
    uint32_t surfaceFormatIndex = std::numeric_limits<uint32_t>::max();
    for (uint32_t index = 0; index < 64; index += 1) {
        if ((info->surfaceFormatBits & (1 << index)) != 0) {
            if (!_adapter->enumerateSurfaceFormats(_surface, index, &surfaceFormat)) {
                continue;
            }

            surfaceFormatIndex = index;
            break;
        }
    }

    if (surfaceFormatIndex == std::numeric_limits<uint32_t>::max()) {
        return Result::ErrorUnknown;
    }

    AdapterLimits limits;
    _adapter->queryLimits(&limits);

    std::vector<uint32_t> indices(limits.queueFamilyCount);

    SwapchainInfo actualInfo = *info;
    actualInfo.preTransform = lowestFlag(info->preTransform);
    actualInfo.compositeAlpha = lowestFlag(info->compositeAlpha);
    actualInfo.surfaceFormatBits = (1 << surfaceFormatIndex);
    actualInfo.presentModeFlags = lowestFlag(info->presentModeFlags);

    /* TODO: better flag selection */
    VkSwapchainCreateInfoKHR vkCreateInfo = {};
    vkCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    vkCreateInfo.surface = _surface->handle<VkSurfaceKHR>();
    vkCreateInfo.minImageCount = info->backbufferCount;
    vkCreateInfo.imageFormat = castEnum<VkFormat>(info->backbufferInfo.format);
    vkCreateInfo.imageColorSpace = castEnum<VkColorSpaceKHR>(surfaceFormat.colorSpaceFlags);
    vkCreateInfo.imageExtent.width = info->backbufferInfo.dimensions.extent.width;
    vkCreateInfo.imageExtent.height = info->backbufferInfo.dimensions.extent.height;
    vkCreateInfo.imageArrayLayers = info->backbufferInfo.dimensions.subresource.layers;
    vkCreateInfo.imageUsage = castEnum<VkImageUsageFlags>(info->backbufferInfo.usage);
    vkCreateInfo.imageSharingMode = (info->backbufferInfo.queueConcurrency ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE);
    if (!info->backbufferInfo.queueConcurrency) {
        /* TODO: possibly more guaranteeable listing of all supported queue families? */
        for (uint32_t i = 0; i < limits.queueFamilyCount; i += 1) {
            indices[i] = i;
        }

        vkCreateInfo.queueFamilyIndexCount = limits.queueFamilyCount;
        vkCreateInfo.pQueueFamilyIndices = &indices[0];
    }

    vkCreateInfo.preTransform = castEnum<VkSurfaceTransformFlagBitsKHR>(actualInfo.preTransform);
    vkCreateInfo.compositeAlpha = castEnum<VkCompositeAlphaFlagBitsKHR>(actualInfo.compositeAlpha);
    vkCreateInfo.presentMode = castEnum<VkPresentModeKHR>(actualInfo.presentModeFlags);
    vkCreateInfo.clipped = info->clipped;
    vkCreateInfo.oldSwapchain = _swapchainData.vkSwapchain;

    PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR = _device->loadDispatchSymbol<PFN_vkCreateSwapchainKHR>("vkCreateSwapchainKHR");
    PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR = _device->loadDispatchSymbol<PFN_vkDestroySwapchainKHR>("vkDestroySwapchainKHR");
    if (vkCreateSwapchainKHR == nullptr || vkDestroySwapchainKHR == nullptr) {
        return Result::ErrorUnknown;
    }

    VkSwapchainKHR vkNewSwapchain;
    Result result = castEnum<Result>(vkCreateSwapchainKHR(_swapchainData.deviceData.vkDevice, &vkCreateInfo, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkNewSwapchain));
    if (result != Result::Success) {
        return result;
    }

    vkDestroySwapchainKHR(_swapchainData.deviceData.vkDevice, _swapchainData.vkSwapchain, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks);

    _swapchainData.backbufferCount = info->backbufferCount;
    _swapchainData.vkSwapchain = vkNewSwapchain;

    _info = actualInfo;

    return result;
}

Result VulkanManualSwapchain::acquireNextIndex(SemaphorePoint const* signalSemaphore, IFence* signalFence, uint32_t* index, uint64_t timeout) noexcept {
    bool timeline = false;
    uint64_t vkSemaphoreValue = 0;
    VkSemaphore vkProvidedSemaphore = VK_NULL_HANDLE;
    if (signalSemaphore != nullptr) {
        if (signalSemaphore->semaphore->handleType() != ObjectType::Semaphore) {
            return Result::ErrorUnknown;
        }

        vkProvidedSemaphore = signalSemaphore->semaphore->handle<VkSemaphore>();
        vkSemaphoreValue = signalSemaphore->value;

        timeline = (signalSemaphore->semaphore->queryInterface<ITimelineSemaphore>() != nullptr);
    }

    VkFence vkFence = VK_NULL_HANDLE;
    if (signalFence != nullptr) {
        if (signalFence->handleType() != ObjectType::Fence) {
            return Result::ErrorUnknown;
        }

        vkFence = signalFence->handle<VkFence>();
    }

    PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR = _device->loadDispatchSymbol<PFN_vkAcquireNextImageKHR>("vkAcquireNextImageKHR");
    if (vkAcquireNextImageKHR == nullptr) {
        return Result::ErrorUnsupportedFeature;
    }

    PFN_vkQueueSubmit vkQueueSubmit = _device->loadDispatchSymbol<PFN_vkQueueSubmit>("vkQueueSubmit");
    VkQueue vkDummySubmissionQueue = VK_NULL_HANDLE;
    VkSemaphore vkBinarySemaphore = vkProvidedSemaphore;
    DummyWSISynchronizationPrimitives dummyPrimitives = {};
    if (timeline) {
        if (!acquireDummyWSISync(dummyPrimitives)) {
            return Result::ErrorUnsupportedFeature;
        }

        vkBinarySemaphore = dummyPrimitives.vkBinarySemaphore;

        AdapterLimits adapterLimits;
        _adapter->queryLimits(&adapterLimits);

        uint32_t firstPresentQueueFamily = std::numeric_limits<uint32_t>::max();
        for (uint32_t i = 0; i < adapterLimits.queueFamilyCount; i += 1) {
            if ((_adapter->queryQueueFamilyFlags(i) & QueueFlags::Present) != QueueFlags::None) {
                firstPresentQueueFamily = i;
                break;
            }
        }

        if (firstPresentQueueFamily == std::numeric_limits<uint32_t>::max()) {
            return Result::ErrorUnsupportedFeature;
        }

        _swapchainData.deviceData.functionPointers.device10.vkGetDeviceQueue(_swapchainData.deviceData.vkDevice, firstPresentQueueFamily, 0, &vkDummySubmissionQueue);

        if (vkQueueSubmit == nullptr) {
            return Result::ErrorUnsupportedFeature;
        }
    }

    Result result = castEnum<Result>(vkAcquireNextImageKHR(_swapchainData.deviceData.vkDevice, _swapchainData.vkSwapchain, timeout, vkBinarySemaphore, vkFence, index));
    if (!timeline) {
        return result;
    }

    if (result != Result::Success && result != Result::NotReady && result != Result::SuboptimalSwapchain && result != Result::Timeout) {
        releaseDummyWSISync(dummyPrimitives);
        return result;
    }

    uint64_t vkBinarySemaphoreWaitValue = 0;

    VkTimelineSemaphoreSubmitInfo timelineSubmitInfo = {};
    timelineSubmitInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
    timelineSubmitInfo.waitSemaphoreValueCount = 1;
    timelineSubmitInfo.pWaitSemaphoreValues = &vkBinarySemaphoreWaitValue;
    timelineSubmitInfo.signalSemaphoreValueCount = 1;
    timelineSubmitInfo.pSignalSemaphoreValues = &vkSemaphoreValue;

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

    VkSubmitInfo dummySubmitInfo = {};
    dummySubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    dummySubmitInfo.pNext = &timelineSubmitInfo;
    dummySubmitInfo.waitSemaphoreCount = 1;
    dummySubmitInfo.pWaitSemaphores = &dummyPrimitives.vkBinarySemaphore;
    dummySubmitInfo.pWaitDstStageMask = &waitStage;
    dummySubmitInfo.signalSemaphoreCount = 1;
    dummySubmitInfo.pSignalSemaphores = &vkProvidedSemaphore;

    Result queueResult = castEnum<Result>(vkQueueSubmit(vkDummySubmissionQueue, 1, &dummySubmitInfo, dummyPrimitives.vkTriggeredFence));
    if (queueResult != Result::Success) {
        return queueResult;
    }

    return result;
}

/* IHandled */
uint64_t VulkanManualSwapchain::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_swapchainData.vkSwapchain);
}

ObjectType VulkanManualSwapchain::handleType() const noexcept {
    return ObjectType::SwapchainKHR;
}

void const* VulkanManualSwapchain::vkData() const noexcept {
    return &_swapchainData;
}

/* IChild */
IParent* VulkanManualSwapchain::parent() const noexcept {
    return _device->queryInterface<IParent>();
}

/* IInterface */
void* VulkanManualSwapchain::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IParent::iid()) {
        return static_cast<IParent*>(this);
    } else if (iid == ISwapchain::iid()) {
        return static_cast<ISwapchain*>(this);
    }

    return nullptr;
}

/* internal */
bool VulkanManualSwapchain::acquireDummyWSISync(DummyWSISynchronizationPrimitives& primitives) {
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (_swapchainData.deviceData.functionPointers.device10.vkCreateSemaphore(_swapchainData.deviceData.vkDevice, &semaphoreCreateInfo, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks, &primitives.vkBinarySemaphore) != VK_SUCCESS) {
        return false;
    }

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    if (_swapchainData.deviceData.functionPointers.device10.vkCreateFence(_swapchainData.deviceData.vkDevice, &fenceCreateInfo, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks, &primitives.vkTriggeredFence) != VK_SUCCESS) {
        _swapchainData.deviceData.functionPointers.device10.vkDestroySemaphore(_swapchainData.deviceData.vkDevice, primitives.vkBinarySemaphore, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
        return false;
    }

    return true;
}

void VulkanManualSwapchain::releaseDummyWSISync(DummyWSISynchronizationPrimitives const& primitives) {
    _swapchainData.deviceData.functionPointers.device10.vkDestroyFence(_swapchainData.deviceData.vkDevice, primitives.vkTriggeredFence, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    _swapchainData.deviceData.functionPointers.device10.vkDestroySemaphore(_swapchainData.deviceData.vkDevice, primitives.vkBinarySemaphore, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
}

}

}