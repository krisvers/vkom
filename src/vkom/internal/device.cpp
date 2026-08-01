#include <vkom/internal/device.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/queue.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

namespace vkom {

namespace internal {

VulkanDevice::VulkanDevice(bool debug, bool inheritedHandle, VulkanAdapter* adapter, VkDevice vkDevice, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanDeviceFunctionPointers const& functionPointers, std::vector<const char*> const& enabledExtensions) : _debug(debug), _inheritedHandle(inheritedHandle), _adapter(adapter), _instance(static_cast<VulkanInstance*>(adapter->parent())), _vkDevice(vkDevice), _vkGetDeviceProcAddr(vkGetDeviceProcAddr), _vkAllocationCallbacks(vkAllocationCallbacks), _functionPointers(functionPointers), _enabledExtensions(enabledExtensions) {
    uint32_t queueFamilyCount;
    _adapter->_functionPointers.physical10.vkGetPhysicalDeviceQueueFamilyProperties(_adapter->_vkPhysicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    _adapter->_functionPointers.physical10.vkGetPhysicalDeviceQueueFamilyProperties(_adapter->_vkPhysicalDevice, &queueFamilyCount, &queueFamilyProperties[0]);

    _queueFamilies.resize(queueFamilyCount);
    for (uint32_t i = 0; i < queueFamilyCount; i += 1) {
        _queueFamilies[i].flags = castEnum<QueueFlags>(queueFamilyProperties[i].queueFlags) | (_adapter->queueFamilySupportsPresent(i) ? QueueFlags::Present : QueueFlags::None);
        _queueFamilies[i].properties = queueFamilyProperties[i];
        _queueFamilies[i].queues = {};
    }
}

VulkanDevice::~VulkanDevice() {
    waitIdle();

    for (IChild* child : _children) {
        if (child->supportsInterface(ICOLLECTED_IID)) {
            ICollected* collected = reinterpret_cast<ICollected*>(child);
            if (collected->release() != 0) {
                /* TODO: report mismanaged references */
            }
        }
    }

    if (!_inheritedHandle && _functionPointers.device10.vkDestroyDevice != nullptr) {
        _functionPointers.device10.vkDestroyDevice(_vkDevice, _vkAllocationCallbacks);
    }
}

/* IDevice */
Result VulkanDevice::waitIdle() const noexcept {
    return castEnum<Result>(_functionPointers.device10.vkDeviceWaitIdle(_vkDevice));
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

        for (VulkanQueue* queue : _queueFamilies[family].queues) {
            if (queue->index() != index) {
                break;
            }
        }
    }

    VulkanQueueFunctionPointers functionPointers = {};
    if (!functionPointers.queue10.load(_vkDevice, _vkGetDeviceProcAddr)) {
        return Result::ErrorInitializationFailed;
    }

    VkQueue vkQueue;
    _functionPointers.device10.vkGetDeviceQueue(_vkDevice, family, index, &vkQueue);

    try {
        *queue = new VulkanQueue(_debug, false, this, family, index, _queueFamilies[family].flags, vkQueue, _vkAllocationCallbacks, functionPointers);
    } catch (std::runtime_error const& err) {
        return Result::ErrorInitializationFailed;
    }

    return Result::Success;
}

IHeap* VulkanDevice::defaultHeap() noexcept {
    return nullptr;
}

Result VulkanDevice::createHeap(BufferUsageFlags bufferUsages, TextureUsageFlags textureUsages, MemoryLocationFlags memoryLocation, IHeap** heap) noexcept {
    return Result::ErrorUnknown;
}

/* INullable */
bool VulkanDevice::isNull() const noexcept {
    return (_vkDevice == nullptr);
}

/* IHandled */
uint64_t VulkanDevice::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_vkDevice);
}

ObjectType VulkanDevice::handleType() const noexcept {
    return ObjectType::Device;
}

/* ICollected */
uint32_t VulkanDevice::release() {
    if (_referenceCount == 0) {
        return 0;
    }

    _referenceCount -= 1;
    if (_referenceCount == 0) {
        delete this;
        return 0;
    }

    return _referenceCount;
}

uint32_t VulkanDevice::retain() {
    _referenceCount += 1;
    return _referenceCount;
}

/* IParent */
bool VulkanDevice::hasChild(IChild const* child) const noexcept {
    for (IChild const* c : _children) {
        if (c == child) {
            return true;
        }
    }

    return false;
}

IChild* VulkanDevice::enumerateChildren(uint32_t id) const noexcept {
    if (id >= _children.size()) {
        return nullptr;
    }

    return _children[id];
}

/* IChild */
IParent* VulkanDevice::parent() const noexcept {
    return _adapter;
}

/* IDispatchable */
void* VulkanDevice::loadDispatchSymbol(const char* symbol) {
    return _instance->loadDispatchSymbol(symbol);
}

/* IInterface */
bool VulkanDevice::supportsInterface(IID const& iid) const noexcept {
    return IDevice::supportsInterface(iid);
}

}

}
