#include <vkom/internal/device.hpp>

#include <limits>
#include <bitset>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/heap.hpp>
#include <vkom/internal/queue.hpp>
#include <vkom/internal/fence.hpp>
#include <vkom/internal/semaphore.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

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
    adopt(_defaultHeap);
}

VulkanDevice::~VulkanDevice() {
    waitIdle();
    ParentByVector::disownAll();
    _adapter->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        vmaDestroyAllocator(_deviceData.vmaAllocator);
        if (_deviceData.functionPointers.device10.vkDestroyDevice != nullptr) {
            _deviceData.functionPointers.device10.vkDestroyDevice(_deviceData.vkDevice, _deviceData.adapterData.instanceData.vkAllocationCallbacks);
        }
    }
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
    return _defaultHeap->queryInterface<IHeap>();
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
    }

    return nullptr;
}

}

}
