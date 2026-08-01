#include <vkom/internal/cmdbatch.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/cmdencoder.hpp>
#include <vkom/internal/queue.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

namespace vkom {

namespace internal {

VulkanCommandBatch::VulkanCommandBatch(bool debug, bool inheritedHandle, VulkanQueue* queue, VkCommandBuffer vkCommandBuffer, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanCommandBatchFunctionPointers const& functionPointers) : _debug(debug), _inheritedHandle(inheritedHandle), _queue(queue), _device(static_cast<VulkanDevice*>(_queue->parent())), _adapter(static_cast<VulkanAdapter*>(_device->parent())), _instance(static_cast<VulkanInstance*>(_adapter->parent())), _vkCommandBuffer(vkCommandBuffer), _vkAllocationCallbacks(vkAllocationCallbacks), _functionPointers(functionPointers) {
    if (functionPointers.commandBuffer10.vkResetCommandBuffer(_vkCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset command buffer");
    }

    /* TODO: some form of render pass/sub pass inheritance */
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (functionPointers.commandBuffer10.vkBeginCommandBuffer(_vkCommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer");
    }
}

VulkanCommandBatch::~VulkanCommandBatch() {
    if (!_ended && _functionPointers.commandBuffer10.vkEndCommandBuffer(_vkCommandBuffer) != VK_SUCCESS) {

    }

    _queue->releaseCommandBuffer(_vkCommandBuffer);
}

/* ICommandBatch */
Result VulkanCommandBatch::record(ICommandEncoder* encoder) noexcept {
    if (_ended) {
        return Result::ErrorUnknown;
    }

    VulkanCommandEncoder* vulkanEncoder = reinterpret_cast<VulkanCommandEncoder*>(encoder);
    _functionPointers.commandBuffer10.vkCmdExecuteCommands(_vkCommandBuffer, 1, &vulkanEncoder->_vkCommandBuffer);
    return Result::Success;
}

Result VulkanCommandBatch::submit(CommandBatchSubmitInfo const* submitInfo) noexcept {
    if (!_ended) {
        Result result = castEnum<Result>(_functionPointers.commandBuffer10.vkEndCommandBuffer(_vkCommandBuffer));
        if (result != Result::Success) {
            return result;
        }

        _ended = true;
    }

    VkSubmitInfo vkSubmitInfo = {};
    vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    vkSubmitInfo.commandBufferCount = 1;
    vkSubmitInfo.pCommandBuffers = &_vkCommandBuffer;

    return castEnum<Result>(_queue->_functionPointers.queue10.vkQueueSubmit(_queue->_vkQueue, 1, &vkSubmitInfo, VK_NULL_HANDLE));
}

void VulkanCommandBatch::discard() noexcept {
    delete this;
}

/* INullable */
bool VulkanCommandBatch::isNull() const noexcept {
    return (_vkCommandBuffer != nullptr);
}

/* IHandled */
uint64_t VulkanCommandBatch::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_vkCommandBuffer);
}

ObjectType VulkanCommandBatch::handleType() const noexcept {
    return ObjectType::CommandBuffer;
}

/* IChild */
IParent* VulkanCommandBatch::parent() const noexcept {
    return _queue;
}

/* IDispatchable */
void* VulkanCommandBatch::loadDispatchSymbol(const char* symbol) {
    return _queue->loadDispatchSymbol(symbol);
}

/* IInterface */
bool VulkanCommandBatch::supportsInterface(IID const& iid) const noexcept {
    return ICommandBatch::supportsInterface(iid);
}

}

}