#include <vkom/internal/device.hpp>

#include <stdexcept>
#include <vector>
#include <limits>
#include <bitset>
#include <cstring>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/heap.hpp>
#include <vkom/internal/queue.hpp>
#include <vkom/internal/fence.hpp>
#include <vkom/internal/semaphore.hpp>
#include <vkom/internal/swapchain.hpp>
#include <vkom/internal/pipeline.hpp>
#include <vkom/internal/descriptor.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>
#include <vkom/internal/format.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vma.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

VulkanDevice::VulkanDevice(bool inheritedHandle, IAdapter* adapter, VulkanDeviceData const& deviceData, std::vector<const char*> const& enabledExtensions) : _inheritedHandle(inheritedHandle), _adapter(adapter), _instance(_adapter->parent<IInstance>()), _deviceData(deviceData), _enabledExtensions(enabledExtensions) {
    uint32_t queueFamilyCount;
    _deviceData.adapterData.functionPointers.physical10.vkGetPhysicalDeviceQueueFamilyProperties(_deviceData.adapterData.vkPhysicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    _deviceData.adapterData.functionPointers.physical10.vkGetPhysicalDeviceQueueFamilyProperties(_deviceData.adapterData.vkPhysicalDevice, &queueFamilyCount, &queueFamilyProperties[0]);

    _queueFamilies.resize(queueFamilyCount);
    for (uint32_t i = 0; i < queueFamilyCount; i += 1) {
        _queueFamilies[i].flags = _adapter->queryQueueFamilyFlags(i);
        _queueFamilies[i].properties = queueFamilyProperties[i];
        _queueFamilies[i].queues = {};
    }

    VulkanHeapData heapData = VulkanHeapData(_deviceData, nullptr);
    _defaultHeap = new VulkanHeap(false, IInterface::queryInterface<IDevice>(), heapData);
    label(_defaultHeap, fmt::label(this, _defaultHeap, "default heap").c_str());
    adopt(_defaultHeap);

    AdapterInfo adapterInfo;
    _adapter->queryInfo(&adapterInfo);

    label(_adapter, fmt::label(this, _adapter, "\"{}\"", adapterInfo.deviceName).c_str());
    label(this, fmt::label(this, this).c_str());
    label(nullptr, "[invalid handle]");
}

VulkanDevice::~VulkanDevice() {
    waitIdle();
    _defaultHeap->release();

    ParentByVector::disownAll();
    _adapter->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        vmaDestroyAllocator(_deviceData.vmaAllocator);
        if (_deviceData.functionPointers.device10.vkDestroyDevice != nullptr) {
            _deviceData.functionPointers.device10.vkDestroyDevice(_deviceData.vkDevice, _deviceData.adapterData.instanceData.vkAllocationCallbacks);
        }
    }
}

/* IWSIDevice */
Result VulkanDevice::createSwapchain(ISurface* surface, SwapchainInfo const* info, ISwapchain** swapchain) noexcept {
    /* TODO: more intelligent surface format selection */
    SurfaceFormat surfaceFormat;
    uint32_t surfaceFormatIndex = std::numeric_limits<uint32_t>::max();
    for (uint32_t index = 0; index < 64; index += 1) {
        if ((info->surfaceFormatBits & (1 << index)) != 0) {
            if (!_adapter->enumerateSurfaceFormats(surface, index, &surfaceFormat)) {
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
    vkCreateInfo.surface = surface->handle<VkSurfaceKHR>();
    vkCreateInfo.minImageCount = info->backbufferCount;
    vkCreateInfo.imageFormat = castEnum<VkFormat>(surfaceFormat.format);
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

    PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR = IDispatchable::loadDispatchSymbol<PFN_vkCreateSwapchainKHR>("vkCreateSwapchainKHR");
    PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR = IDispatchable::loadDispatchSymbol<PFN_vkDestroySwapchainKHR>("vkDestroySwapchainKHR");
    if (vkCreateSwapchainKHR == nullptr || vkDestroySwapchainKHR == nullptr) {
        return Result::ErrorUnknown;
    }

    VkSwapchainKHR vkSwapchain;
    Result result = castEnum<Result>(vkCreateSwapchainKHR(_deviceData.vkDevice, &vkCreateInfo, _deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkSwapchain));
    if (result != Result::Success) {
        return result;
    }

    VulkanSwapchainData swapchainData = VulkanSwapchainData(_deviceData, vkSwapchain, actualInfo.backbufferCount);

    try {
        *swapchain = new VulkanManualSwapchain(false, IInterface::queryInterface<IDevice>(), surface, actualInfo, swapchainData);
    } catch (std::runtime_error err) {
        vkDestroySwapchainKHR(_deviceData.vkDevice, vkSwapchain, _deviceData.adapterData.instanceData.vkAllocationCallbacks);
        return Result::ErrorUnknown;
    }

    adopt(*swapchain);
    return result;
}

Result VulkanDevice::createSwapchainAndSurface(SurfaceWSIInfo const* surfaceInfo, SwapchainInfo const* swapchainInfo, ISurface** surface, ISwapchain** swapchain) noexcept {
    IWSIInstance* wsiInstance = _instance->queryInterface<IWSIInstance>();
    if (wsiInstance == nullptr) {
        return Result::ErrorUnsupportedFeature;
    }

    Result result = wsiInstance->createSurface(surfaceInfo, surface);
    if (result != Result::Success) {
        return result;
    }

    result = createSwapchain(*surface, swapchainInfo, swapchain);
    if (result != Result::Success) {
        (*surface)->release();
        return result;
    }

    return Result::Success;
}

/* IDevice */
Result VulkanDevice::waitIdle() const noexcept {
    return castEnum<Result>(_deviceData.functionPointers.device10.vkDeviceWaitIdle(_deviceData.vkDevice));
}

bool VulkanDevice::queryExtension(const char* extension) const noexcept {
    for (const char* s : _enabledExtensions) {
        if (std::strcmp(s, extension) == 0) {
            return true;
        }
    }

    return false;
}

void VulkanDevice::labelHandle(ObjectType handleType, uint64_t handle, const char* name) noexcept {
    PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT = IDispatchable::loadDispatchSymbol<PFN_vkSetDebugUtilsObjectNameEXT>("vkSetDebugUtilsObjectNameEXT");
    if (vkSetDebugUtilsObjectNameEXT == nullptr) {
        return;
    }

    if (handle == 0) {
        return;
    }

    switch (handleType) {
        case ObjectType::VmaPool:
            return;
        default:
            break;
    }

    VkDebugUtilsObjectNameInfoEXT nameInfo = {};
    nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    nameInfo.objectType = castEnum<VkObjectType>(handleType);
    nameInfo.objectHandle = handle;
    nameInfo.pObjectName = name;

    vkSetDebugUtilsObjectNameEXT(_deviceData.vkDevice, &nameInfo);
}

void VulkanDevice::label(IBase* object, const char* name) noexcept {
    _objectLabelTable[object] = name;

    if (object == nullptr) {
        return;
    }

    IHandled* handled = object->queryInterface<IHandled>();
    if (handled != nullptr) {
        labelHandle(handled->handleType(), handled->handle(), name);
    }
}

const char* VulkanDevice::queryLabel(IBase* object) const noexcept {
    auto it = _objectLabelTable.find(object);
    if (it == _objectLabelTable.end()) {
        return nullptr;
    }

    return it->second.c_str();
}

Result VulkanDevice::acquireQueue(uint32_t family, QueueFlags flags, IQueue** queue) noexcept {
    if (family == QUEUE_FAMILY_ANY) {
        for (uint32_t i = 0; i < _queueFamilies.size(); i += 1) {
            /* TODO: possibly do better family selection */
            if ((_queueFamilies[i].flags & flags) == flags && _queueFamilies[i].queues.size() < _queueFamilies[i].properties.queueCount) {
                family = i;
                break;
            }
        }

        if (family == QUEUE_FAMILY_ANY) {
            return Result::ErrorInitializationFailed;
        }
    }

    if (_queueFamilies[family].queues.size() >= _queueFamilies[family].properties.queueCount) {
        return Result::ErrorTooManyObjects;
    }

    uint32_t index;
    for (index = 0; index < _queueFamilies[family].properties.queueCount; index += 1) {
        if (_queueFamilies[family].queues.size() == 0) {
            break;
        }

        for (IQueue* queue : _queueFamilies[family].queues) {
            if (queue->index() != index) {
                break;
            }
        }
    }

    VulkanQueueFunctionPointers functionPointers = {};
    if (!functionPointers.queue10.load(_deviceData.vkDevice, _deviceData.vkGetDeviceProcAddr)) {
        return Result::ErrorInitializationFailed;
    }

    VkQueue vkQueue;
    _deviceData.functionPointers.device10.vkGetDeviceQueue(_deviceData.vkDevice, family, index, &vkQueue);

    VulkanQueueData queueData = VulkanQueueData(_deviceData, vkQueue, family, index);

    try {
        *queue = new VulkanQueue(false, IInterface::queryInterface<IDevice>(), queueData);
    } catch (std::runtime_error const& err) {
        return Result::ErrorInitializationFailed;
    }

    adopt(*queue);
    return Result::Success;
}

IHeap* VulkanDevice::defaultHeap() noexcept {
    _defaultHeap->retain();
    return _defaultHeap;
}

Result VulkanDevice::createHeap(BufferUsageFlags bufferUsages, TextureUsageFlags textureUsages, MemoryLocationFlags memoryLocation, IHeap** heap) noexcept {
    uint32_t bufferTypeBits = 0xffffffff;
    if (bufferUsages != BufferUsageFlags::None) {
        uint32_t queueFamily = 0;

        VkBufferCreateInfo dummyCreateInfo = {};
        dummyCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        dummyCreateInfo.size = 256;
        dummyCreateInfo.usage = castEnum<VkBufferUsageFlags>(bufferUsages);
        dummyCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        dummyCreateInfo.queueFamilyIndexCount = 1;
        dummyCreateInfo.pQueueFamilyIndices = &queueFamily;

        VkBuffer vkDummyBuffer;
        if (_deviceData.functionPointers.device10.vkCreateBuffer(_deviceData.vkDevice, &dummyCreateInfo, _deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkDummyBuffer) != VK_SUCCESS) {
            /* TODO: error */
            return Result::ErrorUnknown;
        }

        VkMemoryRequirements memoryRequirements;
        _deviceData.functionPointers.device10.vkGetBufferMemoryRequirements(_deviceData.vkDevice, vkDummyBuffer, &memoryRequirements);

        bufferTypeBits = memoryRequirements.memoryTypeBits;
        _deviceData.functionPointers.device10.vkDestroyBuffer(_deviceData.vkDevice, vkDummyBuffer, _deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    uint32_t textureTypeBits = 0xffffffff;
    if (textureUsages != TextureUsageFlags::None) {
        uint32_t queueFamily = 0;

        VkImageCreateInfo dummyCreateInfo = {};
        dummyCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        dummyCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        dummyCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        dummyCreateInfo.extent.width = 256;
        dummyCreateInfo.extent.height = 256;
        dummyCreateInfo.extent.depth = 1;
        dummyCreateInfo.mipLevels = 1;
        dummyCreateInfo.arrayLayers = 1;
        dummyCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        dummyCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        dummyCreateInfo.usage = castEnum<VkImageUsageFlags>(textureUsages);
        dummyCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        dummyCreateInfo.queueFamilyIndexCount = 1;
        dummyCreateInfo.pQueueFamilyIndices = &queueFamily;
        dummyCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImage vkDummyImage;
        if (_deviceData.functionPointers.device10.vkCreateImage(_deviceData.vkDevice, &dummyCreateInfo, _deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkDummyImage) != VK_SUCCESS) {
            /* TODO: error */
            return Result::ErrorUnknown;
        }

        VkMemoryRequirements memoryRequirements;
        _deviceData.functionPointers.device10.vkGetImageMemoryRequirements(_deviceData.vkDevice, vkDummyImage, &memoryRequirements);

        textureTypeBits = memoryRequirements.memoryTypeBits;
        _deviceData.functionPointers.device10.vkDestroyImage(_deviceData.vkDevice, vkDummyImage, _deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    VkPhysicalDeviceMemoryProperties memoryProperties;
    _deviceData.adapterData.functionPointers.physical10.vkGetPhysicalDeviceMemoryProperties(_deviceData.adapterData.vkPhysicalDevice, &memoryProperties);

    VkMemoryPropertyFlags preferredMemoryPropertyFlags = 0;
    if ((memoryLocation & MemoryLocationFlags::GPU) != MemoryLocationFlags::None) {
        preferredMemoryPropertyFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    } else if ((memoryLocation & MemoryLocationFlags::CPU) != MemoryLocationFlags::None) {
        preferredMemoryPropertyFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }

    if ((memoryLocation & MemoryLocationFlags::CPU) != MemoryLocationFlags::None) {
        preferredMemoryPropertyFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    }

    uint32_t typeIndex = std::numeric_limits<uint32_t>::max();
    for (size_t i = 0; i < memoryProperties.memoryTypeCount; i += 1) {
        if (((1 << i) & bufferTypeBits) == 0) {
            continue;
        }

        if (((1 << i) & textureTypeBits) == 0) {
            continue;
        }

        size_t currentCompatibility = std::bitset<32>(memoryProperties.memoryTypes[i].propertyFlags & preferredMemoryPropertyFlags).count();
        size_t previousCompatibility = 0;
        if (typeIndex != std::numeric_limits<uint32_t>::max()) {
            previousCompatibility = std::bitset<32>(memoryProperties.memoryTypes[typeIndex].propertyFlags & preferredMemoryPropertyFlags).count();
        }

        if (currentCompatibility > previousCompatibility) {
            typeIndex = i;
        }
    }

    if (typeIndex == std::numeric_limits<uint32_t>::max()) {
        /* TODO: error */
        return Result::ErrorUnknown;
    }

    VmaPoolCreateInfo createInfo = {};
    createInfo.memoryTypeIndex = typeIndex;

    VmaPool vmaPool;
    if (vmaCreatePool(_deviceData.vmaAllocator, &createInfo, &vmaPool) != VK_SUCCESS) {
        /* TODO: error */
        return Result::ErrorUnknown;
    }

    VulkanHeapData heapData = VulkanHeapData(_deviceData, vmaPool);

    try {
        *heap = new VulkanHeap(false, IInterface::queryInterface<IDevice>(), heapData);
    } catch (std::runtime_error err) {
        vmaDestroyPool(_deviceData.vmaAllocator, vmaPool);
        return Result::ErrorUnknown;
    }

    adopt(*heap);
    return Result::Success;
}

Result VulkanDevice::acquireSemaphore(bool timeline, ISemaphore** semaphore) noexcept {
    AdapterFeatures adapterFeatures = {};
    _adapter->queryFeatures(&adapterFeatures);

    if (timeline && !adapterFeatures.timelineSemaphores) {
        return Result::ErrorUnsupportedFeature;
    }

    VkSemaphoreTypeCreateInfo typeInfo = {};
    typeInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    typeInfo.semaphoreType = (timeline ? VK_SEMAPHORE_TYPE_TIMELINE : VK_SEMAPHORE_TYPE_BINARY);
    typeInfo.initialValue = 0;

    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (timeline) {
        createInfo.pNext = &typeInfo;

        if (!queryExtension(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
            return Result::ErrorUnsupportedFeature;
        }
    }

    VkSemaphore vkSemaphore;
    VkResult result = _deviceData.functionPointers.device10.vkCreateSemaphore(_deviceData.vkDevice, &createInfo, _deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkSemaphore);
    if (result != VK_SUCCESS) {
        return castEnum<Result>(result);
    }

    VulkanSemaphoreData semaphoreData = VulkanSemaphoreData(_deviceData, typeInfo.semaphoreType, vkSemaphore);

    try {
        *semaphore = new VulkanSemaphore(false, this, semaphoreData);
    } catch (std::runtime_error err) {
        _deviceData.functionPointers.device10.vkDestroySemaphore(_deviceData.vkDevice, vkSemaphore, _deviceData.adapterData.instanceData.vkAllocationCallbacks);
        return Result::ErrorUnknown;
    }

    adopt(*semaphore);
    return Result::Success;
}

Result VulkanDevice::acquireFence(bool signaled, IFence** fence) noexcept {
    VkFenceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createInfo.flags = (signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0);

    VkFence vkFence;
    VkResult result = _deviceData.functionPointers.device10.vkCreateFence(_deviceData.vkDevice, &createInfo, _deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkFence);
    if (result != VK_SUCCESS) {
        return castEnum<Result>(result);
    }

    VulkanFenceData fenceData = VulkanFenceData(_deviceData, signaled, vkFence);

    try {
        *fence = new VulkanFence(false, this, fenceData);
    } catch (std::runtime_error err) {
        _deviceData.functionPointers.device10.vkDestroyFence(_deviceData.vkDevice, vkFence, _deviceData.adapterData.instanceData.vkAllocationCallbacks);
        return Result::ErrorUnknown;
    }

    adopt(*fence);
    return Result::Success;
}

Result VulkanDevice::createShaderModule(ShaderModuleInfo const* info, IShaderModule** shader) noexcept {
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = info->length * 4;
    createInfo.pCode = info->spirv;

    VkShaderModule vkShaderModule;
    Result result = castEnum<Result>(_deviceData.functionPointers.device10.vkCreateShaderModule(_deviceData.vkDevice, &createInfo, _deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkShaderModule));
    if (result != Result::Success) {
        return result;
    }

    VulkanShaderModuleData moduleData = VulkanShaderModuleData(_deviceData, vkShaderModule);

    try {
        *shader = new VulkanShaderModule(false, this, moduleData);
    } catch (std::runtime_error err) {
        _deviceData.functionPointers.device10.vkDestroyShaderModule(_deviceData.vkDevice, vkShaderModule, _deviceData.adapterData.instanceData.vkAllocationCallbacks);
        return Result::ErrorUnknown;
    }

    adopt(*shader);
    return Result::Success;
}

Result VulkanDevice::createDescriptorSetLayout(DescriptorSetLayoutInfo const* info, IDescriptorSetLayout** layout) noexcept {
    std::vector<VkSampler> vkImmutableSamplers = {};
    std::vector<VkDescriptorSetLayoutBinding> bindingCreateInfos(info->bindingCount);
    for (uint32_t i = 0; i < info->bindingCount; i += 1) {
        size_t immutableSamplersStart = vkImmutableSamplers.size();
        for (uint32_t j = 0; j < info->bindings[i].count; j += 1) {
            if (info->bindings[i].immutableSamplers == nullptr) {
                break;
            }

            ISampler* sampler = info->bindings[i].immutableSamplers[j];
            if (sampler->handleType() != ObjectType::Sampler) {
                /* TODO: error */
                return Result::ErrorUnknown;
            }

            vkImmutableSamplers.push_back(sampler->handle<VkSampler>());
        }

        bindingCreateInfos[i].binding = info->bindings[i].binding;
        bindingCreateInfos[i].descriptorType = castEnum<VkDescriptorType>(info->bindings[i].flags);
        bindingCreateInfos[i].descriptorCount = info->bindings[i].count;
        bindingCreateInfos[i].stageFlags = castEnum<VkShaderStageFlags>(info->bindings[i].stages);
        bindingCreateInfos[i].pImmutableSamplers = ((vkImmutableSamplers.size() == immutableSamplersStart) ? nullptr : &vkImmutableSamplers[immutableSamplersStart]);
    }

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.flags = castEnum<VkDescriptorSetLayoutCreateFlags>(info->flags);
    createInfo.bindingCount = static_cast<uint32_t>(bindingCreateInfos.size());
    createInfo.pBindings = (bindingCreateInfos.empty() ? nullptr : &bindingCreateInfos[0]);

    VkDescriptorSetLayout vkDescriptorSetLayout;
    Result result = castEnum<Result>(_deviceData.functionPointers.device10.vkCreateDescriptorSetLayout(_deviceData.vkDevice, &createInfo, _deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkDescriptorSetLayout));
    if (result != Result::Success) {
        return result;
    }

    VulkanDescriptorSetLayoutData layoutData = VulkanDescriptorSetLayoutData(_deviceData, vkDescriptorSetLayout);

    try {
        *layout = new VulkanDescriptorSetLayout(false, this, *info, layoutData);
    } catch (std::runtime_error err) {
        /* TODO: error */
        _deviceData.functionPointers.device10.vkDestroyDescriptorSetLayout(_deviceData.vkDevice, vkDescriptorSetLayout, _deviceData.adapterData.instanceData.vkAllocationCallbacks);
        return Result::ErrorUnknown;
    }

    adopt(*layout);
    return Result::Success;
}

Result VulkanDevice::createDescriptorPool(DescriptorPoolInfo const* info, IDescriptorPool** pool) noexcept {
    std::vector<VkDescriptorPoolSize> vkPoolSizes(info->descriptorCount);
    for (uint32_t i = 0; i < info->descriptorCount; i += 1) {
        vkPoolSizes[i].type = castEnum<VkDescriptorType>(info->descriptors[i].flags);
        vkPoolSizes[i].descriptorCount = info->descriptors[i].count;
    }

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.flags = castEnum<VkDescriptorPoolCreateFlags>(info->flags);
    createInfo.maxSets = info->maxDescriptorSets;
    createInfo.poolSizeCount = static_cast<uint32_t>(vkPoolSizes.size());
    createInfo.pPoolSizes = (vkPoolSizes.empty() ? nullptr : &vkPoolSizes[0]);

    VkDescriptorPool vkDescriptorPool;
    Result result = castEnum<Result>(_deviceData.functionPointers.device10.vkCreateDescriptorPool(_deviceData.vkDevice, &createInfo, _deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkDescriptorPool));
    if (result != Result::Success) {
        return result;
    }

    VulkanDescriptorPoolData poolData = VulkanDescriptorPoolData(_deviceData, vkDescriptorPool);

    try {
        *pool = new VulkanDescriptorPool(false, this, *info, poolData);
    } catch (std::runtime_error err) {
        /* TODO: error */
        _deviceData.functionPointers.device10.vkDestroyDescriptorPool(_deviceData.vkDevice, vkDescriptorPool, _deviceData.adapterData.instanceData.vkAllocationCallbacks);
        return Result::ErrorUnknown;
    }

    adopt(*pool);
    return Result::Success;
}

Result VulkanDevice::createPipelineLayout(PipelineLayoutInfo const* info, IPipelineLayout** layout) noexcept {
    std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts(info->descriptorSetLayoutCount);
    for (uint32_t i = 0; i < info->descriptorSetLayoutCount; i += 1) {
        if (info->descriptorSetLayouts[i]->handleType() != ObjectType::DescriptorSetLayout) {
            /* TODO: error */
            return Result::ErrorUnknown;
        }

        vkDescriptorSetLayouts[i] = info->descriptorSetLayouts[i]->handle<VkDescriptorSetLayout>();
    }

    std::vector<VkPushConstantRange> pushConstantRanges(info->pushConstantRangeCount);
    for (uint32_t i = 0; i < info->pushConstantRangeCount; i += 1) {
        pushConstantRanges[i].stageFlags = castEnum<VkShaderStageFlags>(info->pushConstantRanges[i].stages);
        pushConstantRanges[i].offset = info->pushConstantRanges[i].offset;
        pushConstantRanges[i].size = info->pushConstantRanges[i].size;
    }

    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = static_cast<uint32_t>(vkDescriptorSetLayouts.size());
    createInfo.pSetLayouts = (vkDescriptorSetLayouts.empty() ? nullptr : &vkDescriptorSetLayouts[0]);
    createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
    createInfo.pPushConstantRanges = (pushConstantRanges.empty() ? nullptr : &pushConstantRanges[0]);

    VkPipelineLayout vkPipelineLayout;
    Result result = castEnum<Result>(_deviceData.functionPointers.device10.vkCreatePipelineLayout(_deviceData.vkDevice, &createInfo, _deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkPipelineLayout));
    if (result != Result::Success) {
        return result;
    }

    VulkanPipelineLayoutData layoutData = VulkanPipelineLayoutData(_deviceData, vkPipelineLayout);

    try {
        *layout = new VulkanPipelineLayout(false, this, layoutData);
    } catch (std::runtime_error err) {
        _deviceData.functionPointers.device10.vkDestroyPipelineLayout(_deviceData.vkDevice, vkPipelineLayout, _deviceData.adapterData.instanceData.vkAllocationCallbacks);
        return Result::ErrorUnknown;
    }

    adopt(*layout);
    return Result::Success;
}

Result VulkanDevice::createGraphicsPipeline(GraphicsPipelineInfo const* info, IPipelineCache* cache, IPipelineLayout* layout, IGraphicsPipeline** pipeline) noexcept {
    return Result::ErrorUnknown;

    VkPipeline vkPipeline;
    /* TODO: Result result = _deviceData.functionPointers.device10.vkCreateGraphicsPipelines(_deviceData.vkDevice, ) */

    VulkanPipelineData pipelineData = VulkanPipelineData(_deviceData, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);

    try {
        *pipeline = new VulkanPipeline(false, this, pipelineData);
    } catch (std::runtime_error err) {
        _deviceData.functionPointers.device10.vkDestroyPipeline(_deviceData.vkDevice, vkPipeline, _deviceData.adapterData.instanceData.vkAllocationCallbacks);
        return Result::ErrorUnknown;
    }

    adopt(*pipeline);
    return Result::Success;
}

Result VulkanDevice::createComputePipeline(ComputePipelineInfo const* info, IPipelineCache* cache, IPipelineLayout* layout, IComputePipeline** pipeline) noexcept {
    VkPipelineCache vkPipelineCache = VK_NULL_HANDLE;
    if (cache != nullptr) {
        if (cache->handleType() != ObjectType::PipelineCache) {
            /* TODO: error */
            return Result::ErrorUnknown;
        }

        vkPipelineCache = cache->handle<VkPipelineCache>();
    }

    VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
    if (layout != nullptr) {
        if (layout->handleType() != ObjectType::PipelineLayout) {
            /* TODO: error */
            return Result::ErrorUnknown;
        }

        vkPipelineLayout = layout->handle<VkPipelineLayout>();
    }

    VkPipeline vkBasePipeline = VK_NULL_HANDLE;
    if (info->derivationInfo.base != nullptr) {
        if (info->derivationInfo.base->handleType() != ObjectType::Pipeline) {
            /* TODO: error */
            return Result::ErrorUnknown;
        }

        vkBasePipeline = info->derivationInfo.base->handle<VkPipeline>();
    }

    std::vector<VkSpecializationMapEntry> specializationEntries(info->shaderInfo.specialization.entryCount);
    for (uint32_t i = 0; i < info->shaderInfo.specialization.entryCount; i += 1) {
        specializationEntries[i].constantID = info->shaderInfo.specialization.entries[i].constantID;
        specializationEntries[i].offset = info->shaderInfo.specialization.entries[i].offset;
        specializationEntries[i].size = static_cast<size_t>(info->shaderInfo.specialization.entries[i].size);
    }

    VkSpecializationInfo specializationInfo = {};
    specializationInfo.mapEntryCount = static_cast<uint32_t>(specializationEntries.size());
    specializationInfo.pMapEntries = (specializationEntries.empty() ? nullptr : &specializationEntries[0]);
    specializationInfo.dataSize = static_cast<size_t>(info->shaderInfo.specialization.size);
    specializationInfo.pData = info->shaderInfo.specialization.data;

    VkComputePipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    createInfo.flags = castEnum<VkPipelineCreateFlags>(info->flags);
    createInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.stage.flags = castEnum<VkPipelineShaderStageCreateFlags>(info->shaderInfo.flags);
    createInfo.stage.stage = castEnum<VkShaderStageFlagBits>(info->shaderInfo.stage);
    createInfo.stage.module = info->shaderInfo.shader->handle<VkShaderModule>();
    createInfo.stage.pName = info->shaderInfo.entry;
    if (info->shaderInfo.specialization.entryCount != 0) {
        createInfo.stage.pSpecializationInfo = &specializationInfo;
    }

    createInfo.layout = vkPipelineLayout;
    createInfo.basePipelineHandle = vkBasePipeline;
    createInfo.basePipelineIndex = info->derivationInfo.index;

    VkPipeline vkPipeline;
    Result result = castEnum<Result>(_deviceData.functionPointers.device10.vkCreateComputePipelines(_deviceData.vkDevice, vkPipelineCache, 1, &createInfo, _deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkPipeline));
    if (result != Result::Success) {
        return result;
    }

    VulkanPipelineData pipelineData = VulkanPipelineData(_deviceData, VK_PIPELINE_BIND_POINT_COMPUTE, vkPipeline);

    try {
        *pipeline = new VulkanPipeline(false, this, pipelineData);
    } catch (std::runtime_error err) {
        _deviceData.functionPointers.device10.vkDestroyPipeline(_deviceData.vkDevice, vkPipeline, _deviceData.adapterData.instanceData.vkAllocationCallbacks);
        return Result::ErrorUnknown;
    }

    adopt(*pipeline);
    return Result::Success;
}

/* IHandled */
uint64_t VulkanDevice::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_deviceData.vkDevice);
}

ObjectType VulkanDevice::handleType() const noexcept {
    return ObjectType::Device;
}

void const* VulkanDevice::vkData() const noexcept {
    return &_deviceData;
}

/* IChild */
IParent* VulkanDevice::parent() const noexcept {
    return _adapter->queryInterface<IParent>();
}

/* IDispatchable */
void* VulkanDevice::loadDispatchSymbol(const char* symbol) {
    return reinterpret_cast<void*>(_deviceData.vkGetDeviceProcAddr(_deviceData.vkDevice, symbol));
}

/* IInterface */
void* VulkanDevice::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IParent::iid()) {
        return static_cast<IParent*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IDispatchable::iid()) {
        return static_cast<IDispatchable*>(this);
    } else if (iid == IDevice::iid()) {
        return static_cast<IDevice*>(this);
    } else if (iid == IWSIDevice::iid() && queryExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
        return static_cast<IWSIDevice*>(this);
    }

    return nullptr;
}

}

}
