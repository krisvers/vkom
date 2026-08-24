#include <vkom/internal/cmdpasses.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/cmdencoder.hpp>
#include <vkom/internal/queue.hpp>
#include <vkom/internal/descriptor.hpp>
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

VulkanComputePass::VulkanComputePass(ICommandEncoder* encoder, VulkanComputePassData const& passData) : _encoder(encoder), _queue(_encoder->parent<IQueue>()), _device(_queue->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _passData(passData) {
    encoder->retain();

    _device->label(this, fmt::label(_device, this).c_str());
}

VulkanComputePass::~VulkanComputePass() {
    _encoder->disown(IInterface::queryInterface<IChild>());
    _encoder->release();
}

/* IComputePass */
void VulkanComputePass::dispatch(uint32_t width, uint32_t height, uint32_t depth) noexcept {
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdDispatch(_passData.encoderData.vkCommandBuffer, width, height, depth);
}

void VulkanComputePass::dispatchIndirect(IIndirectBuffer* buffer, uint32_t offset) noexcept {
    if (buffer->handleType() != ObjectType::Buffer) {
        /* TODO: error */
        return;
    }

    VkBuffer vkBuffer = buffer->handle<VkBuffer>();
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdDispatchIndirect(_passData.encoderData.vkCommandBuffer, vkBuffer, offset);
}

/* IPass */
void VulkanComputePass::bindPipeline(IPipeline* pipeline) noexcept {
    if (pipeline->queryInterface<IComputePipeline>() == nullptr) {
        /* TODO: warn */
        return;
    }

    VkPipeline vkPipeline = pipeline->handle<VkPipeline>();
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdBindPipeline(_passData.encoderData.vkCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vkPipeline);
}

void VulkanComputePass::bindDescriptorSet(IPipelineLayout* layout, uint32_t id, IDescriptorSet* descriptorSet, uint32_t dynamicOffsetCount, uint32_t const* dynamicOffsets) noexcept {
    if (layout->handleType() != ObjectType::PipelineLayout) {
        /* TODO: error */
        return;
    }

    if (descriptorSet->handleType() != ObjectType::DescriptorSet) {
        /* TODO: error */
        return;
    }

    VkPipelineLayout vkPipelineLayout = layout->handle<VkPipelineLayout>();
    VkDescriptorSet vkDescriptorSet = descriptorSet->handle<VkDescriptorSet>();

    _passData.encoderData.functionPointers.commandBuffer10.vkCmdBindDescriptorSets(_passData.encoderData.vkCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vkPipelineLayout, id, 1, &vkDescriptorSet, dynamicOffsetCount, dynamicOffsets);
}

void VulkanComputePass::pushConstants(IPipelineLayout* layout, ShaderStageFlags stages, uint32_t offset, uint32_t size, void const* data) noexcept {
    VkPipelineLayout vkPipelineLayout = layout->handle<VkPipelineLayout>();
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdPushConstants(_passData.encoderData.vkCommandBuffer, vkPipelineLayout, castEnum<VkShaderStageFlags>(stages), offset, size, data);
}

void VulkanComputePass::end() noexcept {
    delete this;
}

/* IChild */
IParent* VulkanComputePass::parent() const noexcept {
    return _encoder->queryInterface<IParent>();
}

/* IInterface */
void* VulkanComputePass::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IPass::iid()) {
        return static_cast<IPass*>(this);
    } else if (iid == IComputePass::iid()) {
        return static_cast<IComputePass*>(this);
    }

    return nullptr;
}

VulkanRenderPass::VulkanRenderPass(ICommandEncoder* encoder, VulkanRenderPassData const& passData) : _encoder(encoder), _queue(_encoder->parent<IQueue>()), _device(_queue->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _passData(passData) {
    encoder->retain();
}

VulkanRenderPass::~VulkanRenderPass() {
    _encoder->disown(IInterface::queryInterface<IChild>());
    _encoder->release();
}

/* IRenderPass */
void VulkanRenderPass::nextSubpass() noexcept {
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdNextSubpass(_passData.encoderData.vkCommandBuffer, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
}

void VulkanRenderPass::clearColorTarget(uint32_t id, ColorTargetClear const* clear) noexcept {
    VkClearAttachment vkClear = {};
    vkClear.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    vkClear.colorAttachment = id;
    vkClear.clearValue.color.float32[0] = clear->color[0];
    vkClear.clearValue.color.float32[1] = clear->color[1];
    vkClear.clearValue.color.float32[2] = clear->color[2];
    vkClear.clearValue.color.float32[3] = clear->color[3];

    VkClearRect vkRect = {};
    vkRect.rect.offset.x = clear->offset.xyz.x;
    vkRect.rect.offset.y = clear->offset.xyz.y;
    vkRect.rect.extent.width = clear->range.extent.width;
    vkRect.rect.extent.height = clear->range.extent.height;
    vkRect.baseArrayLayer = clear->offset.subresource.layer;
    vkRect.layerCount = clear->range.subresource.layers;

    _passData.encoderData.functionPointers.commandBuffer10.vkCmdClearAttachments(_passData.encoderData.vkCommandBuffer, 1, &vkClear, 1, &vkRect);
}

void VulkanRenderPass::clearDepthStencilTarget(DepthStencilTargetClear const* clear) noexcept {
    VkClearAttachment vkClear = {};
    vkClear.aspectMask = castEnum<VkImageAspectFlags>(clear->aspectFlags);
    vkClear.clearValue.depthStencil.depth = clear->depth;
    vkClear.clearValue.depthStencil.stencil = clear->stencil;

    VkClearRect vkRect = {};
    vkRect.rect.offset.x = clear->offset.xyz.x;
    vkRect.rect.offset.y = clear->offset.xyz.y;
    vkRect.rect.extent.width = clear->range.extent.width;
    vkRect.rect.extent.height = clear->range.extent.height;
    vkRect.baseArrayLayer = clear->offset.subresource.layer;
    vkRect.layerCount = clear->range.subresource.layers;

    _passData.encoderData.functionPointers.commandBuffer10.vkCmdClearAttachments(_passData.encoderData.vkCommandBuffer, 1, &vkClear, 1, &vkRect);
}

void VulkanRenderPass::setViewport(uint32_t index, Viewport const* viewport) noexcept {
    VkViewport vkViewport = {};
    vkViewport.x = viewport->x;
    vkViewport.y = viewport->y;
    vkViewport.width = viewport->width;
    vkViewport.height = viewport->height;
    vkViewport.minDepth = viewport->near;
    vkViewport.maxDepth = viewport->far;

    _passData.encoderData.functionPointers.commandBuffer10.vkCmdSetViewport(_passData.encoderData.vkCommandBuffer, index, 1, &vkViewport);
}

void VulkanRenderPass::setScissor(uint32_t index, Scissor const* scissor) noexcept {
    VkRect2D vkScissor = {};
    vkScissor.offset.x = scissor->x;
    vkScissor.offset.y = scissor->y;
    vkScissor.extent.width = scissor->width;
    vkScissor.extent.height = scissor->height;

    _passData.encoderData.functionPointers.commandBuffer10.vkCmdSetScissor(_passData.encoderData.vkCommandBuffer, index, 1, &vkScissor);
}

void VulkanRenderPass::setLineWidth(float lineWidth) noexcept {
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdSetLineWidth(_passData.encoderData.vkCommandBuffer, lineWidth);
}

void VulkanRenderPass::setDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) noexcept {
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdSetDepthBias(_passData.encoderData.vkCommandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

void VulkanRenderPass::setBlendConstants(float const blendConstants[4]) noexcept {
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdSetBlendConstants(_passData.encoderData.vkCommandBuffer, blendConstants);
}

void VulkanRenderPass::setDepthBounds(float near, float far) noexcept {
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdSetDepthBounds(_passData.encoderData.vkCommandBuffer, near, far);
}

void VulkanRenderPass::setStencilCompareMask(StencilFaceFlags faces, uint32_t compareMask) noexcept {
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdSetStencilCompareMask(_passData.encoderData.vkCommandBuffer, castEnum<VkStencilFaceFlags>(faces), compareMask);
}

void VulkanRenderPass::setStencilWriteMask(StencilFaceFlags faces, uint32_t writeMask) noexcept {
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdSetStencilWriteMask(_passData.encoderData.vkCommandBuffer, castEnum<VkStencilFaceFlags>(faces), writeMask);

}

void VulkanRenderPass::setStencilReference(StencilFaceFlags faces, uint32_t reference) noexcept {
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdSetStencilReference(_passData.encoderData.vkCommandBuffer, castEnum<VkStencilFaceFlags>(faces), reference);
}

void VulkanRenderPass::bindIndexBuffer(IIndexBuffer* buffer, uint64_t offset, IndexType type) noexcept {
    if (buffer->handleType() != ObjectType::Buffer) {
        /* TODO: error */
        return;
    }

    VkBuffer vkBuffer = buffer->handle<VkBuffer>();
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdBindIndexBuffer(_passData.encoderData.vkCommandBuffer, vkBuffer, static_cast<VkDeviceSize>(offset), castEnum<VkIndexType>(type));
}

void VulkanRenderPass::bindVertexBuffer(uint32_t binding, IVertexBuffer* buffer, uint64_t offset) noexcept {
    if (buffer->handleType() != ObjectType::Buffer) {
        /* TODO: error */
        return;
    }

    VkBuffer vkBuffer = buffer->handle<VkBuffer>();
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdBindVertexBuffers(_passData.encoderData.vkCommandBuffer, binding, 1, &vkBuffer, &offset);
}


void VulkanRenderPass::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) noexcept {
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdDraw(_passData.encoderData.vkCommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanRenderPass::drawIndexed(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) noexcept {
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdDrawIndexed(_passData.encoderData.vkCommandBuffer, vertexCount, instanceCount, firstIndex, vertexOffset, firstInstance);

}

void VulkanRenderPass::drawIndirect(IIndirectBuffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) noexcept {
    if (buffer->handleType() != ObjectType::Buffer) {
        /* TODO: error */
        return;
    }

    VkBuffer vkBuffer = buffer->handle<VkBuffer>();
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdDrawIndirect(_passData.encoderData.vkCommandBuffer, vkBuffer, static_cast<VkDeviceSize>(offset), drawCount, stride);
}

void VulkanRenderPass::drawIndirectIndexed(IIndirectBuffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) noexcept {
    if (buffer->handleType() != ObjectType::Buffer) {
        /* TODO: error */
        return;
    }

    VkBuffer vkBuffer = buffer->handle<VkBuffer>();
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdDrawIndexedIndirect(_passData.encoderData.vkCommandBuffer, vkBuffer, static_cast<VkDeviceSize>(offset), drawCount, stride);
}

/* IPass */
void VulkanRenderPass::bindPipeline(IPipeline* pipeline) noexcept {
    if (pipeline->queryInterface<IGraphicsPipeline>() == nullptr) {
        /* TODO: warn */
        return;
    }

    VkPipeline vkPipeline = pipeline->handle<VkPipeline>();
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdBindPipeline(_passData.encoderData.vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
}

void VulkanRenderPass::bindDescriptorSet(IPipelineLayout* layout, uint32_t id, IDescriptorSet* descriptorSet, uint32_t dynamicOffsetCount, uint32_t const* dynamicOffsets) noexcept {
    if (layout->handleType() != ObjectType::PipelineLayout) {
        /* TODO: error */
        return;
    }

    if (descriptorSet->handleType() != ObjectType::DescriptorSet) {
        /* TODO: error */
        return;
    }

    VkPipelineLayout vkPipelineLayout = layout->handle<VkPipelineLayout>();
    VkDescriptorSet vkDescriptorSet = descriptorSet->handle<VkDescriptorSet>();

    _passData.encoderData.functionPointers.commandBuffer10.vkCmdBindDescriptorSets(_passData.encoderData.vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, id, 1, &vkDescriptorSet, dynamicOffsetCount, dynamicOffsets);
}

void VulkanRenderPass::pushConstants(IPipelineLayout* layout, ShaderStageFlags stages, uint32_t offset, uint32_t size, void const* data) noexcept {
    VkPipelineLayout vkPipelineLayout = layout->handle<VkPipelineLayout>();
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdPushConstants(_passData.encoderData.vkCommandBuffer, vkPipelineLayout, castEnum<VkShaderStageFlags>(stages), offset, size, data);
}

void VulkanRenderPass::end() noexcept {
    if (_passData.vkRenderPass != VK_NULL_HANDLE) {
        _passData.encoderData.functionPointers.commandBuffer10.vkCmdEndRenderPass(_passData.encoderData.vkCommandBuffer);
    } else {
        PFN_vkCmdEndRendering vkCmdEndRendering = _device->loadDispatchSymbol<PFN_vkCmdEndRendering>("vkCmdEndRendering");
        if (vkCmdEndRendering == nullptr) {
            vkCmdEndRendering = _device->loadDispatchSymbol<PFN_vkCmdEndRendering>("vkCmdEndRenderingKHR");
        }

        if (vkCmdEndRendering == nullptr) {
            vkCmdEndRendering = _device->loadDispatchSymbol<PFN_vkCmdEndRendering>("vkCmdEndRenderingEXT");
        }

        if (vkCmdEndRendering == nullptr) {
            /* TODO: error */
        } else {
            vkCmdEndRendering(_passData.encoderData.vkCommandBuffer);
        }
    }

    delete this;
}

/* IChild */
IParent* VulkanRenderPass::parent() const noexcept {
    return _encoder->queryInterface<IParent>();
}

/* IInterface */
void* VulkanRenderPass::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IPass::iid()) {
        return static_cast<IPass*>(this);
    } else if (iid == IRenderPass::iid()) {
        return static_cast<IRenderPass*>(this);
    }

    return nullptr;
}

}

}
