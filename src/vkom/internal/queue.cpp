#include <vkom/internal/queue.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/cmdencoder.hpp>
#include <vkom/internal/cmdbatch.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>
#include <vkom/internal/format.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

VulkanQueueEvent::VulkanQueueEvent(bool inheritedHandle, IQueue* queue, VulkanQueueEventData const& eventData) : _inheritedHandle(inheritedHandle), _queue(queue), _device(_queue->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _eventData(eventData) {
    _queue->retain();

    _device->label(this, fmt::label(_device, this).c_str());
}

VulkanQueueEvent::~VulkanQueueEvent() {
    _queue->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        _eventData.queueData.deviceData.functionPointers.device10.vkDestroyEvent(_eventData.queueData.deviceData.vkDevice, _eventData.vkEvent, _eventData.queueData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    _queue->release();
}

/* IQueueEvent */
Result VulkanQueueEvent::set(bool signaled) noexcept {
    if (signaled) {
        return castEnum<Result>(_eventData.queueData.deviceData.functionPointers.device10.vkSetEvent(_eventData.queueData.deviceData.vkDevice, _eventData.vkEvent));
    }

    return castEnum<Result>(_eventData.queueData.deviceData.functionPointers.device10.vkResetEvent(_eventData.queueData.deviceData.vkDevice, _eventData.vkEvent));
}

bool VulkanQueueEvent::status() const noexcept {
    return (_eventData.queueData.deviceData.functionPointers.device10.vkGetEventStatus(_eventData.queueData.deviceData.vkDevice, _eventData.vkEvent) == VK_EVENT_SET);
}

/* IHandled */
uint64_t VulkanQueueEvent::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_eventData.vkEvent);
}

ObjectType VulkanQueueEvent::handleType() const noexcept {
    return ObjectType::Event;
}

void const* VulkanQueueEvent::vkData() const noexcept {
    return &_eventData;
}

/* IChild */
IParent* VulkanQueueEvent::parent() const noexcept {
    return _queue->queryInterface<IParent>();
}

/* IInterface */
void* VulkanQueueEvent::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IParent::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IQueueEvent::iid()) {
        return static_cast<IQueueEvent*>(this);
    }

    return nullptr;
}

VulkanQueue::VulkanQueue(bool inheritedHandle, IDevice* device, VulkanQueueData const& queueData) : _inheritedHandle(inheritedHandle), _device(device), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _queueData(queueData), _flags(_adapter->queryQueueFamilyFlags(_queueData.family)) {
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = _queueData.family;

    if (_queueData.deviceData.functionPointers.device10.vkCreateCommandPool(_queueData.deviceData.vkDevice, &createInfo, _queueData.deviceData.adapterData.instanceData.vkAllocationCallbacks, &_vkCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("vkCreateCommandPool failed");
    }

    _device->retain();

    _device->label(this, fmt::label(_device, this, "({}:{}) [{}]", family(), index(), flags()).c_str());
}

VulkanQueue::~VulkanQueue() {
    waitIdle();
    ParentByVector::disownAll();
    _device->disown(IInterface::queryInterface<IChild>());

    _queueData.deviceData.functionPointers.device10.vkDestroyCommandPool(_queueData.deviceData.vkDevice, _vkCommandPool, _queueData.deviceData.adapterData.instanceData.vkAllocationCallbacks);

    _device->release();
}

/* IQueue */
uint32_t VulkanQueue::family() const noexcept {
    return _queueData.family;
}

uint32_t VulkanQueue::index() const noexcept {
    return _queueData.index;
}

QueueFlags VulkanQueue::flags() const noexcept {
    return _flags;
}

Result VulkanQueue::waitIdle() const noexcept {
    return castEnum<Result>(_queueData.functionPointers.queue10.vkQueueWaitIdle(_queueData.vkQueue));
}

Result VulkanQueue::acquireQueueEvent(IQueueEvent** event) noexcept {
    VkEventCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;

    VkEvent vkEvent;
    Result result = castEnum<Result>(_queueData.deviceData.functionPointers.device10.vkCreateEvent(_queueData.deviceData.vkDevice, &createInfo, _queueData.deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkEvent));
    if (result != Result::Success) {
        return result;
    }

    VulkanQueueEventData eventData = VulkanQueueEventData(_queueData, vkEvent);

    try {
        *event = new VulkanQueueEvent(false, this, eventData);
    } catch (std::runtime_error err) {
        /* TODO: error */
        _queueData.deviceData.functionPointers.device10.vkDestroyEvent(_queueData.deviceData.vkDevice, vkEvent, _queueData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
        return Result::ErrorUnknown;
    }

    adopt(*event);
    return Result::Success;
}

Result VulkanQueue::acquireCommandEncoder(ICommandEncoder** encoder) noexcept {
    VkCommandBuffer vkCommandBuffer = acquireCommandBuffer(true);
    if (vkCommandBuffer == nullptr) {
        return Result::ErrorInitializationFailed;
    }

    VulkanCommandEncoderData encoderData = VulkanCommandEncoderData(_queueData, _vkCommandPool, vkCommandBuffer, false);

    *encoder = new VulkanCommandEncoder(false, IInterface::queryInterface<IQueue>(), encoderData);
    adopt(*encoder);
    return Result::Success;
}

Result VulkanQueue::acquireCommandBatch(ICommandBatch** batch) noexcept {
    VkCommandBuffer vkCommandBuffer = acquireCommandBuffer(false);
    if (vkCommandBuffer == nullptr) {
        return Result::ErrorInitializationFailed;
    }

    VulkanCommandBatchData batchData = VulkanCommandBatchData(_queueData, _vkCommandPool, vkCommandBuffer, false);

    *batch = new VulkanCommandBatch(false, IInterface::queryInterface<IQueue>(), batchData);
    adopt(*batch);
    return Result::Success;
}

/* IHandled */
uint64_t VulkanQueue::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_queueData.vkQueue);
}

ObjectType VulkanQueue::handleType() const noexcept {
    return ObjectType::Queue;
}

void const* VulkanQueue::vkData() const noexcept {
    return &_queueData;
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
    if (_queueData.deviceData.functionPointers.device10.vkAllocateCommandBuffers(_queueData.deviceData.vkDevice, &allocateInfo, &vkCommandBuffer) != VK_SUCCESS) {
        return nullptr;
    }

    return vkCommandBuffer;
}

}

}
