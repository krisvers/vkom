#include "vkom/instance.hpp"
#include "vkom/interface.hpp"
#include "vkom/internal/vkdata.hpp"
#include <vkom/internal/queue.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/cmdencoder.hpp>
#include <vkom/internal/cmdbatch.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

namespace vkom {

namespace internal {

VulkanQueue::VulkanQueue(bool inheritedHandle, IDevice* device, VulkanQueueData const& queueData) : _inheritedHandle(inheritedHandle), _device(device), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _queueData(queueData), _flags(_adapter->queryQueueFamilyFlags(_queueData.family)) {
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = _queueData.family;

    if (_queueData.deviceData.functionPointers.device10.vkCreateCommandPool(_queueData.deviceData.vkDevice, &createInfo, _queueData.deviceData.adapterData.instanceData.vkAllocationCallbacks, &_vkCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("vkCreateCommandPool failed");
    }

    _device->retain();
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

Result VulkanQueue::acquireCommandEncoder(ICommandEncoder** encoder) noexcept {
    VkCommandBuffer vkCommandBuffer = acquireCommandBuffer(true);
    if (vkCommandBuffer == nullptr) {
        return Result::ErrorInitializationFailed;
    }

    VulkanCommandEncoderData encoderData = VulkanCommandEncoderData(_queueData, _vkCommandPool, vkCommandBuffer, false);

    *encoder = new VulkanCommandEncoder(false, IInterface::queryInterface<IQueue>(), encoderData);
    return Result::Success;
}

Result VulkanQueue::acquireCommandBatch(ICommandBatch** batch) noexcept {
    VkCommandBuffer vkCommandBuffer = acquireCommandBuffer(false);
    if (vkCommandBuffer == nullptr) {
        return Result::ErrorInitializationFailed;
    }

    VulkanCommandBatchData batchData = VulkanCommandBatchData(_queueData, _vkCommandPool, vkCommandBuffer, false);

    *batch = new VulkanCommandBatch(false, IInterface::queryInterface<IQueue>(), batchData);
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
