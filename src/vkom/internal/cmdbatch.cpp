#include "vkom/enums.hpp"
#include "vkom/interface.hpp"
#include "vkom/object.hpp"
#include "vulkan/vulkan_core.h"
#include <vkom/internal/cmdbatch.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/cmdencoder.hpp>
#include <vkom/internal/queue.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

namespace vkom {

namespace internal {

VulkanCommandBatch::VulkanCommandBatch(bool inheritedHandle, IQueue* queue, VulkanCommandBatchData const& batchData) : _inheritedHandle(inheritedHandle), _queue(queue), _device(_queue->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _batchData(batchData) {
    if (_batchData.functionPointers.commandBuffer10.vkResetCommandBuffer(_batchData.vkCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset command buffer");
    }

    /* TODO: some form of render pass/sub pass inheritance */
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (_batchData.functionPointers.commandBuffer10.vkBeginCommandBuffer(_batchData.vkCommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer");
    }
}

VulkanCommandBatch::~VulkanCommandBatch() {
    _queue->waitIdle();
    _queue->disown(IInterface::queryInterface<IChild>());

    if (!_ended && _batchData.functionPointers.commandBuffer10.vkEndCommandBuffer(_batchData.vkCommandBuffer) != VK_SUCCESS) {

    }

    if (!_inheritedHandle) {
        _batchData.queueData.deviceData.functionPointers.device10.vkFreeCommandBuffers(_batchData.queueData.deviceData.vkDevice, _batchData.vkCommandPool, 1, &_batchData.vkCommandBuffer);
    }
}

/* ICommandBatch */
Result VulkanCommandBatch::record(ICommandEncoder* encoder) noexcept {
    if (_ended) {
        return Result::ErrorUnknown;
    }

    VkCommandBuffer vkCommandBuffer = nullptr;
    if (encoder->handleType() == ObjectType::CommandBuffer) {
        vkCommandBuffer = encoder->handle<VkCommandBuffer>();
    }

    if (vkCommandBuffer == nullptr) {
        return Result::ErrorUnknown;
    }

    _batchData.functionPointers.commandBuffer10.vkCmdExecuteCommands(_batchData.vkCommandBuffer, 1, &vkCommandBuffer);
    return Result::Success;
}

Result VulkanCommandBatch::submit(CommandBatchSubmitInfo const* submitInfo) noexcept {
    if (!_ended) {
        Result result = castEnum<Result>(_batchData.functionPointers.commandBuffer10.vkEndCommandBuffer(_batchData.vkCommandBuffer));
        if (result != Result::Success) {
            return result;
        }

        _ended = true;
    }

    std::vector<VkPipelineStageFlags> vkWaitStageFlags(submitInfo->waitCount);
    std::vector<VkSemaphore> vkWaitSemaphores(submitInfo->waitCount);
    std::vector<uint64_t> vkWaitSemaphoreValues(submitInfo->waitCount);
    
    bool timeline = false;
    for (uint32_t i = 0; i < submitInfo->waitCount; i += 1) {
        vkWaitStageFlags[i] = castEnum<VkPipelineStageFlags>(submitInfo->waits[i].stageFlags);
        vkWaitSemaphores[i] = submitInfo->waits[i].point.semaphore->handle<VkSemaphore>();
        vkWaitSemaphoreValues[i] = submitInfo->waits[i].point.value;

        if (submitInfo->waits[i].point.semaphore->queryInterface<ITimelineSemaphore>() != nullptr) {
            timeline = true;
        }
    }

    std::vector<VkSemaphore> vkSignalSemaphores(submitInfo->signalCount);
    std::vector<uint64_t> vkSignalSemaphoreValues(submitInfo->signalCount);

    for (uint32_t i = 0; i < submitInfo->signalCount; i += 1) {
        vkSignalSemaphores[i] = submitInfo->signals[i].point.semaphore->handle<VkSemaphore>();
        vkSignalSemaphoreValues[i] = submitInfo->signals[i].point.value;

        if (submitInfo->signals[i].point.semaphore->queryInterface<ITimelineSemaphore>() != nullptr) {
            timeline = true;
        }
    }

    AdapterFeatures adapterFeatures = {};
    _adapter->queryFeatures(&adapterFeatures);

    if (timeline && !adapterFeatures.timelineSemaphores) {
        return Result::ErrorUnsupportedFeature;
    }

    VkTimelineSemaphoreSubmitInfo vkTimelineSubmitInfo = {};
    vkTimelineSubmitInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
    vkTimelineSubmitInfo.waitSemaphoreValueCount = submitInfo->waitCount;
    vkTimelineSubmitInfo.pWaitSemaphoreValues = (submitInfo->waitCount == 0 ? nullptr : &vkWaitSemaphoreValues[0]);
    vkTimelineSubmitInfo.signalSemaphoreValueCount = submitInfo->signalCount;
    vkTimelineSubmitInfo.pSignalSemaphoreValues = (submitInfo->signalCount == 0 ? nullptr : &vkSignalSemaphoreValues[0]);

    VkSubmitInfo vkSubmitInfo = {};
    vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    vkSubmitInfo.pNext = (timeline ? &vkTimelineSubmitInfo : nullptr);
    vkSubmitInfo.waitSemaphoreCount = submitInfo->waitCount;
    vkSubmitInfo.pWaitDstStageMask = (submitInfo->waitCount == 0 ? nullptr : &vkWaitStageFlags[0]);
    vkSubmitInfo.pWaitSemaphores = (submitInfo->waitCount == 0 ? nullptr : &vkWaitSemaphores[0]);
    vkSubmitInfo.commandBufferCount = 1;
    vkSubmitInfo.pCommandBuffers = &_batchData.vkCommandBuffer;
    vkSubmitInfo.signalSemaphoreCount = submitInfo->signalCount;
    vkSubmitInfo.pSignalSemaphores = (submitInfo->signalCount == 0 ? nullptr : &vkSignalSemaphores[0]);

    VkFence vkFence = VK_NULL_HANDLE;
    if (submitInfo->signalFence != nullptr) {
        vkFence = submitInfo->signalFence->handle<VkFence>();
    }

    return castEnum<Result>(_batchData.queueData.functionPointers.queue10.vkQueueSubmit(_batchData.queueData.vkQueue, 1, &vkSubmitInfo, vkFence));
}

/* IHandled */
uint64_t VulkanCommandBatch::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_batchData.vkCommandBuffer);
}

ObjectType VulkanCommandBatch::handleType() const noexcept {
    return ObjectType::CommandBuffer;
}

void const* VulkanCommandBatch::vkData() const noexcept {
    return &_batchData;
}

/* IChild */
IParent* VulkanCommandBatch::parent() const noexcept {
    return _queue->queryInterface<IParent>();
}

/* IDispatchable */
void* VulkanCommandBatch::loadDispatchSymbol(const char* symbol) {
    return _queue->loadDispatchSymbol(symbol);
}

/* IInterface */
void* VulkanCommandBatch::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == IDiscardable::iid()) {
        return static_cast<IDiscardable*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IDispatchable::iid()) {
        return static_cast<IDispatchable*>(this);
    } else if (iid == ICommandBatch::iid()) {
        return static_cast<ICommandBatch*>(this);
    }

    return nullptr;
}

}

}
