#include "vkom/internal/vkdata.hpp"
#include <vkom/internal/device.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/heap.hpp>
#include <vkom/internal/queue.hpp>
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
    _defaultHeap = new VulkanHeap(false, this, heapData);
    adopt(_defaultHeap);
}

VulkanDevice::~VulkanDevice() {
    waitIdle();
    ParentByVector::disownAll();

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
        *queue = new VulkanQueue(false, this, queueData);
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

    /* adopt(*heap); */
    return Result::ErrorUnknown;
}

/* IHandled */
uint64_t VulkanDevice::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_deviceData.vkDevice);
}

ObjectType VulkanDevice::handleType() const noexcept {
    return ObjectType::Device;
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
    if (iid == IHandled::iid()) {
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
