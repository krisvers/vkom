#include <vkom/internal/adapter.hpp>

#include <algorithm>
#include <cstring>

#include <vkom/internal/instance.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/enums.hpp>
#include <vkom/internal/vksurface.hpp>

namespace vkom {

namespace internal {

VulkanAdapter::VulkanAdapter(bool debug, VulkanInstance* instance, VkPhysicalDevice vkPhysicalDevice, VulkanAdapterFunctionPointers const& functionPointers) : _debug(debug), _instance(instance), _vkPhysicalDevice(vkPhysicalDevice), _functionPointers(functionPointers) {
    _instance->retain();

    uint32_t availableExtensionCount;
    if (_functionPointers.physical10.vkEnumerateDeviceExtensionProperties(_vkPhysicalDevice, nullptr, &availableExtensionCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("vkEnumerateDeviceExtensionProperties failed");
    }

    _availableExtensions.resize(availableExtensionCount);
    if (_functionPointers.physical10.vkEnumerateDeviceExtensionProperties(_vkPhysicalDevice, nullptr, &availableExtensionCount, &_availableExtensions[0]) != VK_SUCCESS) {
        throw std::runtime_error("vkEnumerateDeviceExtensionProperties failed");
    }

    VkPhysicalDeviceProperties2 properties2 = {};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceDriverProperties driverProperties = {};
    driverProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;
    driverProperties.pNext = properties2.pNext;
    if (instance->vkApiVersion() >= VK_API_VERSION_1_2 || isExtensionAvailable(VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME)) {
        properties2.pNext = &driverProperties;
    }

    if (_functionPointers.physical11.vkGetPhysicalDeviceProperties2 != nullptr) {
        _functionPointers.physical11.vkGetPhysicalDeviceProperties2(_vkPhysicalDevice, &properties2);
    } else {
        _functionPointers.physical10.vkGetPhysicalDeviceProperties(_vkPhysicalDevice, &properties2.properties);
    }

    VkPhysicalDeviceFeatures2 features2 = {};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

    VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures = {};
    dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dynamicRenderingFeatures.pNext = features2.pNext;
    if (instance->vkApiVersion() >= VK_API_VERSION_1_3 || isExtensionAvailable(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)) {
        features2.pNext = &dynamicRenderingFeatures;
    }

    VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {};
    bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    bufferDeviceAddressFeatures.pNext = features2.pNext;
    if (instance->vkApiVersion() >= VK_API_VERSION_1_2 || isExtensionAvailable(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) || isExtensionAvailable(VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
        features2.pNext = &bufferDeviceAddressFeatures;
    }

    VkPhysicalDeviceShaderFloat16Int8Features shaderFloat16Int8Features = {};
    shaderFloat16Int8Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES;
    shaderFloat16Int8Features.pNext = features2.pNext;
    if (instance->vkApiVersion() >= VK_API_VERSION_1_2 || isExtensionAvailable(VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME)) {
        features2.pNext = &shaderFloat16Int8Features;
    }

    VkPhysicalDeviceShaderFloat8FeaturesEXT shaderFloat8Features = {};
    shaderFloat8Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT8_FEATURES_EXT;
    shaderFloat8Features.pNext = features2.pNext;
    if (isExtensionAvailable(VK_EXT_SHADER_FLOAT8_EXTENSION_NAME)) {
        features2.pNext = &shaderFloat8Features;
    }

    if (_functionPointers.physical11.vkGetPhysicalDeviceFeatures2 != nullptr) {
        _functionPointers.physical11.vkGetPhysicalDeviceFeatures2(_vkPhysicalDevice, &features2);
    } else {
        _functionPointers.physical10.vkGetPhysicalDeviceFeatures(_vkPhysicalDevice, &features2.features);
    }

    VkPhysicalDeviceMemoryProperties2 memoryProperties2 = {};
    memoryProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;

    if (_functionPointers.physical11.vkGetPhysicalDeviceMemoryProperties2 != nullptr) {
        _functionPointers.physical11.vkGetPhysicalDeviceMemoryProperties2(_vkPhysicalDevice, &memoryProperties2);
    } else {
        _functionPointers.physical10.vkGetPhysicalDeviceMemoryProperties(_vkPhysicalDevice, &memoryProperties2.memoryProperties);
    }

    _info.vendorID = castEnum<VendorID>(properties2.properties.vendorID);
    _info.driverID = castEnum<DriverID>(driverProperties.driverID);
    _info.deviceID = properties2.properties.deviceID;
    _info.driverVersion = properties2.properties.driverVersion;

    std::memset(&_info.deviceName[0], 0, 256);
    std::memset(&_info.driverName[0], 0, 256);

    std::memcpy(&_info.deviceName[0], &properties2.properties.deviceName[0], std::min(VK_MAX_PHYSICAL_DEVICE_NAME_SIZE - 1, 255u));
    if (instance->vkApiVersion() >= VK_API_VERSION_1_2 || isExtensionAvailable(VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME)) {
        std::memcpy(&_info.driverName[0], &driverProperties.driverName, std::min(VK_MAX_DRIVER_NAME_SIZE - 1, 255u));
    }

    _features.swapchain = isExtensionAvailable(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    _features.dynamicRendering = dynamicRenderingFeatures.dynamicRendering;
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
}

VulkanAdapter::~VulkanAdapter() {
    for (IChild* child : _children) {
        ICollected* collected = child->queryInterface<ICollected>();
        if (collected != nullptr) {
            if (collected->release() != 0) {
                /* TODO: report mismanaged references */
            }
        }
    }

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
    _functionPointers.physical10.vkGetPhysicalDeviceQueueFamilyProperties(_vkPhysicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    _functionPointers.physical10.vkGetPhysicalDeviceQueueFamilyProperties(_vkPhysicalDevice, &queueFamilyCount, &queueFamilies[0]);

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
        }
    }

    PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = static_cast<IDispatchable*>(_instance)->loadDispatchSymbol<PFN_vkGetDeviceProcAddr>("vkGetDeviceProcAddr");
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
    VkResult result = _functionPointers.physical10.vkCreateDevice(_vkPhysicalDevice, &createInfo, _vkAllocationCallbacks, &vkDevice);
    if (result != VK_SUCCESS) {
        return castEnum<Result>(result);
    }

    VulkanDeviceFunctionPointers functionPointers = {};
    if (!functionPointers.device10.load(vkDevice, vkGetDeviceProcAddr)) {
        /* failed to load device function pointers */
        PFN_vkDestroyDevice vkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(vkGetDeviceProcAddr(vkDevice, "vkDestroyDevice"));
        if (vkDestroyDevice != nullptr) {
            vkDestroyDevice(vkDevice, _vkAllocationCallbacks);
        }

        return Result::ErrorInitializationFailed;
    }

    functionPointers.device11.load(vkDevice, vkGetDeviceProcAddr);
    functionPointers.device12.load(vkDevice, vkGetDeviceProcAddr);

    try {
        *device = new VulkanDevice(_debug, false, this, vkDevice, vkGetDeviceProcAddr, _vkAllocationCallbacks, functionPointers, enabledExtensions);
    } catch (std::runtime_error const& err) {
        return Result::ErrorInitializationFailed;
    }

    return Result::Success;
}

/* IHandled */
uint64_t VulkanAdapter::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_vkPhysicalDevice);
}

ObjectType VulkanAdapter::handleType() const noexcept {
    return ObjectType::PhysicalDevice;
}

/* IParent */
bool VulkanAdapter::hasChild(IChild const* child) const noexcept {
    for (IChild const* c : _children) {
        if (c == child) {
            return true;
        }
    }

    return false;
}

IChild* VulkanAdapter::enumerateChildren(uint32_t id) const noexcept {
    if (id >= _children.size()) {
        return nullptr;
    }

    return _children[id];
}

/* IChild */
IParent* VulkanAdapter::parent() const noexcept {
    return _instance;
}

/* IDispatchable */
void* VulkanAdapter::loadDispatchSymbol(const char* symbol) {
    return _instance->loadDispatchSymbol(symbol);
}

/* IInterface */
void* VulkanAdapter::queryInterface(IID const& iid) noexcept {
    if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
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

/* internal */
bool VulkanAdapter::isExtensionAvailable(const char* name) const noexcept {
    for (VkExtensionProperties const& properties : _availableExtensions) {
        if (std::strcmp(properties.extensionName, name) == 0) {
            return true;
        }
    }

    return false;
}

bool VulkanAdapter::queueFamilySupportsPresent(uint32_t family) const noexcept {
    return physicalDeviceQueueFamilySupportsPresentation(_instance->_vkInstance, _instance->_vkGetInstanceProcAddr, _vkPhysicalDevice, family);
}

}

}
