#include "vkom/enums.hpp"
#include "vkom/object.hpp"
#include <cstdint>
#include <vkom/internal/adapter.hpp>

#include <algorithm>
#include <cstring>

#include <vkom/internal/instance.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/enums.hpp>
#include <vkom/internal/vksurface.hpp>
#include <vkom/internal/vma.hpp>

namespace vkom {

namespace internal {

VulkanAdapter::VulkanAdapter(bool inheritedHandle, IInstance* instance, VulkanAdapterData const& adapterData) : _inheritedHandle(inheritedHandle), _instance(instance), _adapterData(adapterData) {
    uint32_t availableExtensionCount;
    if (_adapterData.functionPointers.physical10.vkEnumerateDeviceExtensionProperties(_adapterData.vkPhysicalDevice, nullptr, &availableExtensionCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("vkEnumerateDeviceExtensionProperties failed");
    }

    _availableExtensions.resize(availableExtensionCount);
    if (_adapterData.functionPointers.physical10.vkEnumerateDeviceExtensionProperties(_adapterData.vkPhysicalDevice, nullptr, &availableExtensionCount, &_availableExtensions[0]) != VK_SUCCESS) {
        throw std::runtime_error("vkEnumerateDeviceExtensionProperties failed");
    }

    VkPhysicalDeviceProperties2 properties2 = {};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceDriverProperties driverProperties = {};
    driverProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;
    driverProperties.pNext = properties2.pNext;
    if (instance->queryApiVersion() >= VK_API_VERSION_1_2 || queryExtension(VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME)) {
        properties2.pNext = &driverProperties;
    }

    if (_adapterData.functionPointers.physical11.vkGetPhysicalDeviceProperties2 != nullptr) {
        _adapterData.functionPointers.physical11.vkGetPhysicalDeviceProperties2(_adapterData.vkPhysicalDevice, &properties2);
    } else {
        _adapterData.functionPointers.physical10.vkGetPhysicalDeviceProperties(_adapterData.vkPhysicalDevice, &properties2.properties);
    }

    VkPhysicalDeviceFeatures2 features2 = {};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

    VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures = {};
    dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dynamicRenderingFeatures.pNext = features2.pNext;
    if (instance->queryApiVersion() >= VK_API_VERSION_1_3 || queryExtension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)) {
        features2.pNext = &dynamicRenderingFeatures;
    }

    VkPhysicalDeviceTimelineSemaphoreFeatures timelineSemaphoreFeatures = {};
    timelineSemaphoreFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;
    timelineSemaphoreFeatures.pNext = features2.pNext;
    if (instance->queryApiVersion() >= VK_API_VERSION_1_2 || queryExtension(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
        features2.pNext = &timelineSemaphoreFeatures;
    }

    VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {};
    bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    bufferDeviceAddressFeatures.pNext = features2.pNext;
    if (instance->queryApiVersion() >= VK_API_VERSION_1_2 || queryExtension(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) || queryExtension(VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
        features2.pNext = &bufferDeviceAddressFeatures;
    }

    VkPhysicalDeviceShaderFloat16Int8Features shaderFloat16Int8Features = {};
    shaderFloat16Int8Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES;
    shaderFloat16Int8Features.pNext = features2.pNext;
    if (instance->queryApiVersion() >= VK_API_VERSION_1_2 || queryExtension(VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME)) {
        features2.pNext = &shaderFloat16Int8Features;
    }

    VkPhysicalDeviceShaderFloat8FeaturesEXT shaderFloat8Features = {};
    shaderFloat8Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT8_FEATURES_EXT;
    shaderFloat8Features.pNext = features2.pNext;
    if (queryExtension(VK_EXT_SHADER_FLOAT8_EXTENSION_NAME)) {
        features2.pNext = &shaderFloat8Features;
    }

    if (_adapterData.functionPointers.physical11.vkGetPhysicalDeviceFeatures2 != nullptr) {
        _adapterData.functionPointers.physical11.vkGetPhysicalDeviceFeatures2(_adapterData.vkPhysicalDevice, &features2);
    } else {
        _adapterData.functionPointers.physical10.vkGetPhysicalDeviceFeatures(_adapterData.vkPhysicalDevice, &features2.features);
    }

    VkPhysicalDeviceMemoryProperties2 memoryProperties2 = {};
    memoryProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;

    if (_adapterData.functionPointers.physical11.vkGetPhysicalDeviceMemoryProperties2 != nullptr) {
        _adapterData.functionPointers.physical11.vkGetPhysicalDeviceMemoryProperties2(_adapterData.vkPhysicalDevice, &memoryProperties2);
    } else {
        _adapterData.functionPointers.physical10.vkGetPhysicalDeviceMemoryProperties(_adapterData.vkPhysicalDevice, &memoryProperties2.memoryProperties);
    }

    _info.vendorID = castEnum<VendorID>(properties2.properties.vendorID);
    _info.driverID = castEnum<DriverID>(driverProperties.driverID);
    _info.deviceID = properties2.properties.deviceID;
    _info.driverVersion = properties2.properties.driverVersion;

    std::memset(&_info.deviceName[0], 0, 256);
    std::memset(&_info.driverName[0], 0, 256);

    std::memcpy(&_info.deviceName[0], &properties2.properties.deviceName[0], std::min(VK_MAX_PHYSICAL_DEVICE_NAME_SIZE - 1, 255u));
    if (instance->queryApiVersion() >= VK_API_VERSION_1_2 || queryExtension(VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME)) {
        std::memcpy(&_info.driverName[0], &driverProperties.driverName, std::min(VK_MAX_DRIVER_NAME_SIZE - 1, 255u));
    }

    _features.swapchain = queryExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    _features.dynamicRendering = dynamicRenderingFeatures.dynamicRendering;
    _features.timelineSemaphores = timelineSemaphoreFeatures.timelineSemaphore;
    _features.bufferDeviceAddress = bufferDeviceAddressFeatures.bufferDeviceAddress;
    _features.shaderInt8 = shaderFloat16Int8Features.shaderInt8;
    _features.shaderInt16 = features2.features.shaderInt16;
    _features.shaderInt64 = features2.features.shaderInt64;
    _features.shaderFloat8 = shaderFloat8Features.shaderFloat8;
    _features.shaderFloat16 = shaderFloat16Int8Features.shaderFloat16;
    _features.shaderFloat64 = features2.features.shaderFloat64;

    _limits.maxTextureWidth1D = properties2.properties.limits.maxImageDimension1D;
    _limits.maxTextureWidthHeight2D = properties2.properties.limits.maxImageDimension2D;
    _limits.maxTextureWidthHeightDepth3D = properties2.properties.limits.maxImageDimension3D;
    _limits.maxTextureWidthHeightCube = properties2.properties.limits.maxImageDimensionCube;
    _limits.maxTextureLayers = properties2.properties.limits.maxImageArrayLayers;

    _limits.maxUniformBufferRange = properties2.properties.limits.maxUniformBufferRange;
    _limits.maxStorageBufferRange = properties2.properties.limits.maxStorageBufferRange;
    _limits.maxPushConstantsSize = properties2.properties.limits.maxPushConstantsSize;

    _limits.maxComputeSharedMemorySize = properties2.properties.limits.maxComputeSharedMemorySize;
    _limits.maxComputeWorkGroupCount[0] = properties2.properties.limits.maxComputeWorkGroupCount[0];
    _limits.maxComputeWorkGroupCount[1] = properties2.properties.limits.maxComputeWorkGroupCount[1];
    _limits.maxComputeWorkGroupCount[2] = properties2.properties.limits.maxComputeWorkGroupCount[2];
    _limits.maxComputeWorkGroupInvocations = properties2.properties.limits.maxComputeWorkGroupInvocations;
    _limits.maxComputeWorkGroupSize[0] = properties2.properties.limits.maxComputeWorkGroupSize[0];
    _limits.maxComputeWorkGroupSize[1] = properties2.properties.limits.maxComputeWorkGroupSize[1];
    _limits.maxComputeWorkGroupSize[2] = properties2.properties.limits.maxComputeWorkGroupSize[2];

    _limits.timestampPeriod = properties2.properties.limits.timestampPeriod;

    MemoryLocationFlags supportedHeapLocations[VK_MAX_MEMORY_HEAPS];
    for (uint32_t i = 0; i < memoryProperties2.memoryProperties.memoryTypeCount; i += 1) {
        if ((memoryProperties2.memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0) {
            supportedHeapLocations[memoryProperties2.memoryProperties.memoryTypes[i].heapIndex] |= MemoryLocationFlags::GPU;
        }

        if ((memoryProperties2.memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0) {
            supportedHeapLocations[memoryProperties2.memoryProperties.memoryTypes[i].heapIndex] |= MemoryLocationFlags::CPU;
        }
    }

    _limits.availablePrivateMemory = 0;
    _limits.availableSharedMemory = 0;
    _limits.availableHostMemory = 0;

    for (uint32_t i = 0; i < memoryProperties2.memoryProperties.memoryHeapCount; i += 1) {
        if ((supportedHeapLocations[i] & MemoryLocationFlags::GPU) != MemoryLocationFlags::None) {
            _limits.availablePrivateMemory += static_cast<uint64_t>(memoryProperties2.memoryProperties.memoryHeaps[i].size);
            if ((supportedHeapLocations[i] & MemoryLocationFlags::CPU) != MemoryLocationFlags::None) {
                _limits.availableSharedMemory += static_cast<uint64_t>(memoryProperties2.memoryProperties.memoryHeaps[i].size);
            }
        } else if ((supportedHeapLocations[i] & MemoryLocationFlags::CPU) != MemoryLocationFlags::None) {
            _limits.availableHostMemory += static_cast<uint64_t>(memoryProperties2.memoryProperties.memoryHeaps[i].size);
        }
    }

    _adapterData.functionPointers.physical10.vkGetPhysicalDeviceQueueFamilyProperties(_adapterData.vkPhysicalDevice, &_limits.queueFamilyCount, nullptr);

    _instance->retain();
}

VulkanAdapter::~VulkanAdapter() {
    ParentByVector::disownAll();
    _instance->release();
}

/* IAdapter */
void VulkanAdapter::queryInfo(AdapterInfo* info) const noexcept {
    *info = _info;
}

void VulkanAdapter::queryFeatures(AdapterFeatures* features) const noexcept {
    *features = _features;
}

void VulkanAdapter::queryLimits(AdapterLimits* limits) const noexcept {
    *limits = _limits;
}

bool VulkanAdapter::queryExtension(const char* extension) const noexcept {
    for (VkExtensionProperties const& properties : _availableExtensions) {
        if (std::strcmp(properties.extensionName, extension) == 0) {
            return true;
        }
    }

    return false;
}

QueueFlags VulkanAdapter::queryQueueFamilyFlags(uint32_t family) const noexcept {
    uint32_t queueFamilyCount = _limits.queueFamilyCount;
    if (family >= queueFamilyCount) {
        return QueueFlags::None;
    }

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    _adapterData.functionPointers.physical10.vkGetPhysicalDeviceQueueFamilyProperties(_adapterData.vkPhysicalDevice, &queueFamilyCount, &queueFamilyProperties[0]);

    return (castEnum<QueueFlags>(queueFamilyProperties[family].queueFlags) | (physicalDeviceQueueFamilySupportsPresentation(_adapterData.instanceData.vkInstance, _adapterData.instanceData.vkGetInstanceProcAddr, _adapterData.vkPhysicalDevice, family) ? QueueFlags::Present : QueueFlags::None));
}

Result VulkanAdapter::createDevice(IDevice** device) {
    VkPhysicalDeviceFeatures2 features2 = {};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.features.shaderInt16 = _features.shaderInt16;
    features2.features.shaderInt64 = _features.shaderInt64;
    features2.features.shaderFloat64 = _features.shaderFloat64;

    VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures = {};
    dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dynamicRenderingFeatures.pNext = features2.pNext;
    dynamicRenderingFeatures.dynamicRendering = _features.dynamicRendering;
    if (_features.dynamicRendering) {
        features2.pNext = &dynamicRenderingFeatures;
    }

    VkPhysicalDeviceTimelineSemaphoreFeatures timelineSemaphoreFeatures = {};
    timelineSemaphoreFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;
    timelineSemaphoreFeatures.pNext = features2.pNext;
    timelineSemaphoreFeatures.timelineSemaphore = _features.timelineSemaphores;
    if (_features.timelineSemaphores) {
        features2.pNext = &timelineSemaphoreFeatures;
    }

    VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {};
    bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    bufferDeviceAddressFeatures.pNext = features2.pNext;
    bufferDeviceAddressFeatures.bufferDeviceAddress = _features.bufferDeviceAddress;
    if (_features.bufferDeviceAddress) {
        features2.pNext = &bufferDeviceAddressFeatures;
    }

    VkPhysicalDeviceShaderFloat16Int8Features shaderFloat16Int8Features = {};
    shaderFloat16Int8Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES;
    shaderFloat16Int8Features.pNext = features2.pNext;
    shaderFloat16Int8Features.shaderInt8 = _features.shaderInt8;
    shaderFloat16Int8Features.shaderFloat16 = _features.shaderFloat16;
    if (_features.shaderInt8 || _features.shaderFloat16) {
        features2.pNext = &shaderFloat16Int8Features;
    }

    VkPhysicalDeviceShaderFloat8FeaturesEXT shaderFloat8Features = {};
    shaderFloat8Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT8_FEATURES_EXT;
    shaderFloat8Features.pNext = features2.pNext;
    shaderFloat8Features.shaderFloat8 = _features.shaderFloat8;
    if (_features.shaderFloat8) {
        features2.pNext = &shaderFloat8Features;
    }

    uint32_t queueFamilyCount;
    _adapterData.functionPointers.physical10.vkGetPhysicalDeviceQueueFamilyProperties(_adapterData.vkPhysicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    _adapterData.functionPointers.physical10.vkGetPhysicalDeviceQueueFamilyProperties(_adapterData.vkPhysicalDevice, &queueFamilyCount, &queueFamilies[0]);

    uint32_t totalQueueCount = 0;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(queueFamilyCount);
    for (uint32_t qf = 0; qf < queueFamilyCount; qf += 1) {
        totalQueueCount += queueFamilies[qf].queueCount;
    }

    std::vector<float> queuePriorities(totalQueueCount);

    uint32_t currentTotalQueueIndex = 0;
    for (uint32_t qf = 0; qf < queueFamilyCount; qf += 1) {
        uint32_t lowPriorityCount = queueFamilies[qf].queueCount / 3;
        for (uint32_t i = 0; i < queueFamilies[qf].queueCount; i += 1) {
            if (i < (queueFamilies[qf].queueCount - lowPriorityCount)) {
                /* normal priority */
                queuePriorities[currentTotalQueueIndex + i] = 0.5f;
            } else {
                /* low priority */
                queuePriorities[currentTotalQueueIndex + i] = 0.0f;
            }
        }

        queueCreateInfos[qf] = {};
        queueCreateInfos[qf].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[qf].queueFamilyIndex = qf;
        queueCreateInfos[qf].queueCount = queueFamilies[qf].queueCount;
        queueCreateInfos[qf].pQueuePriorities = &queuePriorities[currentTotalQueueIndex];

        currentTotalQueueIndex += queueFamilies[qf].queueCount;
    }

    void* next = &features2;

    bool enabledExtensionBufferDeviceAddress = false;

    std::vector<const char*> enabledExtensions = {};
    for (VkExtensionProperties const& extension : _availableExtensions) {
        if (std::strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
            enabledExtensions.push_back(extension.extensionName);
        } else if (!enabledExtensionBufferDeviceAddress && std::strcmp(extension.extensionName, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) == 0) {
            enabledExtensions.push_back(extension.extensionName);
            enabledExtensionBufferDeviceAddress = true;
        } else if (!enabledExtensionBufferDeviceAddress && std::strcmp(extension.extensionName, VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) == 0) {
            enabledExtensions.push_back(extension.extensionName);
            enabledExtensionBufferDeviceAddress = true;
        } else if (std::strcmp(extension.extensionName, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME) == 0) {
            enabledExtensions.push_back(extension.extensionName);
        } else if (std::strcmp(extension.extensionName, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME) == 0) {
            enabledExtensions.push_back(extension.extensionName);
        }
    }

    PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = _instance->loadDispatchSymbol<PFN_vkGetDeviceProcAddr>("vkGetDeviceProcAddr");
    if (vkGetDeviceProcAddr == nullptr) {
        /* failed to load vkGetDeviceProcAddr */
        return Result::ErrorInitializationFailed;
    }

    VkDeviceCreateFlags createFlags = {};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.flags = createFlags;
    createInfo.pNext = next;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = &queueCreateInfos[0];
    createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
    createInfo.ppEnabledExtensionNames = enabledExtensions.empty() ? nullptr : &enabledExtensions[0];

    VkDevice vkDevice;
    VkResult result = _adapterData.functionPointers.physical10.vkCreateDevice(_adapterData.vkPhysicalDevice, &createInfo, _adapterData.instanceData.vkAllocationCallbacks, &vkDevice);
    if (result != VK_SUCCESS) {
        return castEnum<Result>(result);
    }

    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = _adapterData.instanceData.vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.flags = (_features.bufferDeviceAddress) ? VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT : 0;
    allocatorCreateInfo.physicalDevice = _adapterData.vkPhysicalDevice;
    allocatorCreateInfo.device = vkDevice;
    allocatorCreateInfo.pAllocationCallbacks = _adapterData.instanceData.vkAllocationCallbacks;
    allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
    allocatorCreateInfo.instance = _adapterData.instanceData.vkInstance;
    allocatorCreateInfo.vulkanApiVersion = _instance->queryApiVersion();

    VmaAllocator vmaAllocator;
    if (vmaCreateAllocator(&allocatorCreateInfo, &vmaAllocator) != VK_SUCCESS) {
        throw std::runtime_error("vmaCreateAllocator failed");
    }

    VulkanDeviceData deviceData = VulkanDeviceData(_adapterData, vkGetDeviceProcAddr, vkDevice, vmaAllocator);
    try {
        *device = new VulkanDevice(false, IInterface::queryInterface<IAdapter>(), deviceData, enabledExtensions);
    } catch (std::runtime_error const& err) {
        return Result::ErrorInitializationFailed;
    }

    return Result::Success;
}

/* IHandled */
uint64_t VulkanAdapter::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_adapterData.vkPhysicalDevice);
}

ObjectType VulkanAdapter::handleType() const noexcept {
    return ObjectType::PhysicalDevice;
}

void const* VulkanAdapter::vkData() const noexcept {
    return &_adapterData;
}

/* IChild */
IParent* VulkanAdapter::parent() const noexcept {
    return _instance->queryInterface<IParent>();
}

/* IDispatchable */
void* VulkanAdapter::loadDispatchSymbol(const char* symbol) {
    return _instance->loadDispatchSymbol(symbol);
}

/* IInterface */
void* VulkanAdapter::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == IDestructible::iid()) {
        return static_cast<IDestructible*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IParent::iid()) {
        return static_cast<IParent*>(this);
    } else if (iid == IDispatchable::iid()) {
        return static_cast<IDispatchable*>(this);
    } else if (iid == IAdapter::iid()) {
        return static_cast<IAdapter*>(this);
    }

    return nullptr;
}

}

}
