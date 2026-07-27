#include <vkom/internal/queue.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

namespace vkom {

namespace internal {

VulkanQueue::VulkanQueue(bool debug, bool inheritedHandle, VulkanDevice* device, uint32_t family, uint32_t index, QueueFlags flags, VkQueue vkQueue, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanQueueFunctionPointers const& functionPointers) : _debug(debug), _inheritedHandle(inheritedHandle), _device(device), _adapter(static_cast<VulkanAdapter*>(_device->parent())), _instance(static_cast<VulkanInstance*>(_adapter->parent())), _family(family), _index(index), _flags(flags), _vkQueue(vkQueue), _vkAllocationCallbacks(vkAllocationCallbacks), _functionPointers(functionPointers) {
    _device->_queueFamilies[_family].queues.push_back(this);
}

VulkanQueue::~VulkanQueue() {
    waitIdle();

    for (size_t i = 0; i < _device->_queueFamilies[_family].queues.size(); i += 1) {
        if (_device->_queueFamilies[_family].queues[i] == this) {
            _device->_queueFamilies[_family].queues.erase(_device->_queueFamilies[_family].queues.begin() + i);
            break;
        }
    }
}

/* IQueue */
uint32_t VulkanQueue::family() const noexcept {
    return _family;
}

uint32_t VulkanQueue::index() const noexcept {
    return _index;
}

QueueFlags VulkanQueue::flags() const noexcept {
    return _flags;
}

Result VulkanQueue::waitIdle() const noexcept {
    return castEnum<Result>(_functionPointers.queue10.vkQueueWaitIdle(_vkQueue));
}

/* INullable */
bool VulkanQueue::isNull() const noexcept {
    return (_vkQueue != nullptr);
}

/* IHandled */
uint64_t VulkanQueue::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_vkQueue);
}

ObjectType VulkanQueue::handleType() const noexcept {
    return ObjectType::Queue;
}

/* ICollected */
uint32_t VulkanQueue::release() {
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

uint32_t VulkanQueue::retain() {
    _referenceCount += 1;
    return _referenceCount;
}

/* IParent */
bool VulkanQueue::hasChild(IChild const* child) const noexcept {
    for (IChild const* c : _children) {
        if (c == child) {
            return true;
        }
    }

    return false;
}

IChild* VulkanQueue::enumerateChildren(uint32_t id) const noexcept {
    if (id >= _children.size()) {
        return nullptr;
    }

    return _children[id];
}

/* IChild */
IParent* VulkanQueue::parent() const noexcept {
    return _device;
}

/* IDispatchable */
void* VulkanQueue::loadDispatchSymbol(const char* symbol) {
    return _device->loadDispatchSymbol(symbol);
}

/* IInterface */
bool VulkanQueue::supportsInterface(IID const& iid) const noexcept {
    return IQueue::supportsInterface(iid);
}

}

}