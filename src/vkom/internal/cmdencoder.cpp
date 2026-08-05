#include "vkom/interface.hpp"
#include "vkom/internal/object.hpp"
#include <vkom/internal/cmdencoder.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/cmdpasses.hpp>
#include <vkom/internal/queue.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

namespace vkom {

namespace internal {

VulkanCommandEncoder::VulkanCommandEncoder(bool inheritedHandle, IQueue* queue, VulkanCommandEncoderData const& encoderData) : _inheritedHandle(inheritedHandle), _queue(queue), _device(_queue->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _encoderData(encoderData) {
    if (_encoderData.functionPointers.commandBuffer10.vkResetCommandBuffer(_encoderData.vkCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset command buffer");
    }

    /* TODO: some form of render pass/sub pass inheritance */
    VkCommandBufferInheritanceInfo inheritanceInfo = {};
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pInheritanceInfo = &inheritanceInfo;

    if (_encoderData.functionPointers.commandBuffer10.vkBeginCommandBuffer(_encoderData.vkCommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer");
    }
}

VulkanCommandEncoder::~VulkanCommandEncoder() {
    _queue->waitIdle();
    _queue->disown(IInterface::queryInterface<IChild>());

    if (_renderPass != nullptr) {
        _renderPass->end();
    }

    ParentByVector::disownAll();

    if (!_ended && _encoderData.functionPointers.commandBuffer10.vkEndCommandBuffer(_encoderData.vkCommandBuffer) != VK_SUCCESS) {

    }

    if (!_inheritedHandle) {
        _encoderData.queueData.deviceData.functionPointers.device10.vkFreeCommandBuffers(_encoderData.queueData.deviceData.vkDevice, _encoderData.vkCommandPool, 1, &_encoderData.vkCommandBuffer);
    }
}

/* ICommandEncoder */
IComputePass* VulkanCommandEncoder::beginComputePass(ComputePassDescriptor const* descriptor) noexcept {
    return nullptr;
}

IRenderPass* VulkanCommandEncoder::beginRenderPass(RenderPassDescriptor const* descriptor) noexcept {
    return nullptr;
}

void VulkanCommandEncoder::insertDebugLabel(const char* label) noexcept {
    if (_encoderData.functionPointers.debugUtilsEXT.vkCmdInsertDebugUtilsLabelEXT == nullptr) {
        return;
    }

    VkDebugUtilsLabelEXT labelInfo = {};
    labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    labelInfo.pLabelName = label;
    labelInfo.color[3] = 1.0f;

    hashLabelToColor(label, labelInfo.color[0], labelInfo.color[1], labelInfo.color[2]);
    _encoderData.functionPointers.debugUtilsEXT.vkCmdInsertDebugUtilsLabelEXT(_encoderData.vkCommandBuffer, &labelInfo);
}

void VulkanCommandEncoder::pushDebugGroup(const char* label) noexcept {
    if (_encoderData.functionPointers.debugUtilsEXT.vkCmdBeginDebugUtilsLabelEXT == nullptr) {
        return;
    }

    VkDebugUtilsLabelEXT labelInfo = {};
    labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    labelInfo.pLabelName = label;
    labelInfo.color[3] = 1.0f;

    hashLabelToColor(label, labelInfo.color[0], labelInfo.color[1], labelInfo.color[2]);
    _encoderData.functionPointers.debugUtilsEXT.vkCmdBeginDebugUtilsLabelEXT(_encoderData.vkCommandBuffer, &labelInfo);
}

void VulkanCommandEncoder::popDebugGroup() noexcept {
    if (_encoderData.functionPointers.debugUtilsEXT.vkCmdEndDebugUtilsLabelEXT == nullptr) {
        return;
    }

    _encoderData.functionPointers.debugUtilsEXT.vkCmdEndDebugUtilsLabelEXT(_encoderData.vkCommandBuffer);
}

void VulkanCommandEncoder::copyBufferToBuffer() noexcept {

}

void VulkanCommandEncoder::copyBufferToImage() noexcept {

}

void VulkanCommandEncoder::copyImageToImage() noexcept {

}

void VulkanCommandEncoder::copyImageToBuffer() noexcept {

}

void VulkanCommandEncoder::smallBufferUpload() noexcept {

}

void VulkanCommandEncoder::fillBuffer() noexcept {

}

void VulkanCommandEncoder::globalMemoryBarrier(GeneralBarrier const* barrier) noexcept {

}

void VulkanCommandEncoder::transitionTexture(TextureTransition const* transition) noexcept {

}

void VulkanCommandEncoder::transitionBuffer(BufferTransition const* transition) noexcept {

}

void VulkanCommandEncoder::transferResourceQueueFamily(ResourceQueueFamilyTransfer const* transfer) noexcept {

}

Result VulkanCommandEncoder::finish() noexcept {
    if (_ended) {
        return Result::Success;
    }

    _ended = true;
    return castEnum<Result>(_encoderData.functionPointers.commandBuffer10.vkEndCommandBuffer(_encoderData.vkCommandBuffer));
}

Result VulkanCommandEncoder::batch(ICommandBatch** batch) noexcept {
    Result result = finish();
    if (result != Result::Success) {
        return result;
    }

    ICommandBatch* internalBatch;
    result = _queue->acquireCommandBatch(&internalBatch);
    if (result != Result::Success) {
        return result;
    }

    result = internalBatch->record(this);
    if (result != Result::Success) {
        internalBatch->discard();
    }

    *batch = internalBatch;
    return Result::Success;
}

/* IHandled */
uint64_t VulkanCommandEncoder::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_encoderData.vkCommandBuffer);
}

ObjectType VulkanCommandEncoder::handleType() const noexcept {
    return ObjectType::CommandBuffer;
}

/* IChild */
IParent* VulkanCommandEncoder::parent() const noexcept {
    return _queue;
}

/* IDispatchable */
void* VulkanCommandEncoder::loadDispatchSymbol(const char* symbol) {
    return _queue->loadDispatchSymbol(symbol);
}

/* IInterface */
void* VulkanCommandEncoder::queryInterface(IID const& iid) noexcept {
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
    } else if (iid == ICommandEncoder::iid()) {
        return static_cast<ICommandEncoder*>(this);
    }

    return nullptr;
}

/* internal */
void VulkanCommandEncoder::hashLabelToColor(const char* label, float& r, float& g, float& b) {
    std::hash<std::string> hasher = {};
    size_t hashed = hasher(label);

    r = ((hashed & 0xffe00000) >> 21) / 2048.0f;
    g = ((hashed & 0x001ffc00) >> 10) / 2048.0f;
    b = ((hashed & 0x000003ff) >> 0) / 1024.0f;
}

}

}
