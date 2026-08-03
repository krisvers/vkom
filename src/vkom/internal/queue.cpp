#include <vkom/internal/queue.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/cmdencoder.hpp>
#include <vkom/internal/cmdbatch.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

namespace vkom {

namespace internal {

VulkanQueue::VulkanQueue(bool debug, bool inheritedHandle, VulkanDevice* device, uint32_t family, uint32_t index, QueueFlags flags, VkQueue vkQueue, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanQueueFunctionPointers const& functionPointers) : _debug(debug), _inheritedHandle(inheritedHandle), _device(device), _adapter(static_cast<VulkanAdapter*>(_device->parent())), _instance(static_cast<VulkanInstance*>(_adapter->parent())), _family(family), _index(index), _flags(flags), _vkQueue(vkQueue), _vkAllocationCallbacks(vkAllocationCallbacks), _functionPointers(functionPointers) {
    _device->_queueFamilies[_family].queues.push_back(this);

    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = _family;

    if (_device->_functionPointers.device10.vkCreateCommandPool(_device->_vkDevice, &createInfo, _vkAllocationCallbacks, &_vkCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("vkCreateCommandPool failed");
    }
}

VulkanQueue::~VulkanQueue() {
    waitIdle();

    for (size_t i = 0; i < _device->_queueFamilies[_family].queues.size(); i += 1) {
        if (_device->_queueFamilies[_family].queues[i] == this) {
            _device->_queueFamilies[_family].queues.erase(_device->_queueFamilies[_family].queues.begin() + i);
            break;
        }
    }

    for (IChild* child : _children) {
        ICollected* collected = child->queryInterface<ICollected>();
        if (collected != nullptr) {
            if (collected->release() != 0) {
                /* TODO: report mismanaged references */
            }
        }
    }

    _device->_functionPointers.device10.vkDestroyCommandPool(_device->_vkDevice, _vkCommandPool, _vkAllocationCallbacks);
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

Result VulkanQueue::acquireCommandEncoder(ICommandEncoder** encoder) noexcept {
    VulkanCommandEncoderFunctionPointers functionPointers = {};
    if (!functionPointers.commandBuffer10.load(_device->_vkDevice, _device->_vkGetDeviceProcAddr)) {
        return Result::ErrorInitializationFailed;
    }

    functionPointers.debugUtilsEXT.load(_device->_vkDevice, _device->_vkGetDeviceProcAddr);

    VkCommandBuffer vkCommandBuffer = acquireCommandBuffer(true);
    if (vkCommandBuffer == nullptr) {
        return Result::ErrorInitializationFailed;
    }

    *encoder = new VulkanCommandEncoder(_debug, _inheritedHandle, this, vkCommandBuffer, _vkAllocationCallbacks, functionPointers);
    return Result::Success;
}

Result VulkanQueue::acquireCommandBatch(ICommandBatch** batch) noexcept {
    VulkanCommandBatchFunctionPointers functionPointers = {};
    if (!functionPointers.commandBuffer10.load(_device->_vkDevice, _device->_vkGetDeviceProcAddr)) {
        return Result::ErrorInitializationFailed;
    }

    functionPointers.debugUtilsEXT.load(_device->_vkDevice, _device->_vkGetDeviceProcAddr);

    VkCommandBuffer vkCommandBuffer = acquireCommandBuffer(false);
    if (vkCommandBuffer == nullptr) {
        return Result::ErrorInitializationFailed;
    }

    *batch = new VulkanCommandBatch(_debug, _inheritedHandle, this, vkCommandBuffer, _vkAllocationCallbacks, functionPointers);
    return Result::Success;
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
void* VulkanQueue::queryInterface(IID const& iid) noexcept {
    if (iid == INullable::iid()) {
        return static_cast<INullable*>(this);
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
    } else if (iid == IQueue::iid()) {
        return static_cast<IQueue*>(this);
    }

    return nullptr;
}

/* internal */
VkCommandBuffer VulkanQueue::acquireCommandBuffer(bool secondary) {
    /* TODO: smarter command buffer allocation scheme */
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = _vkCommandPool;
    allocateInfo.level = (secondary) ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer vkCommandBuffer;
    if (_device->_functionPointers.device10.vkAllocateCommandBuffers(_device->_vkDevice, &allocateInfo, &vkCommandBuffer) != VK_SUCCESS) {
        return nullptr;
    }

    return vkCommandBuffer;
}

void VulkanQueue::releaseCommandBuffer(VkCommandBuffer vkCommandBuffer) {
    /* TODO: fine grained wait for command buffer to leave pending */
    waitIdle();

    _device->_functionPointers.device10.vkFreeCommandBuffers(_device->_vkDevice, _vkCommandPool, 1, &vkCommandBuffer);
}

}

}
