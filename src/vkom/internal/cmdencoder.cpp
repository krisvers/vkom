#include <vkom/internal/cmdencoder.hpp>

#include <stdexcept>
#include <algorithm>
#include <cmath>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/cmdpasses.hpp>
#include <vkom/internal/queue.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vkdata.hpp>

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

    _queue->retain();
}

VulkanCommandEncoder::~VulkanCommandEncoder() {
    _queue->waitIdle();
    ParentByVector::disownAll();
    _queue->disown(IInterface::queryInterface<IChild>());

    if (_renderPass != nullptr) {
        _renderPass->end();
    }

    if (!_ended && _encoderData.functionPointers.commandBuffer10.vkEndCommandBuffer(_encoderData.vkCommandBuffer) != VK_SUCCESS) {

    }

    if (!_inheritedHandle) {
        _encoderData.queueData.deviceData.functionPointers.device10.vkFreeCommandBuffers(_encoderData.queueData.deviceData.vkDevice, _encoderData.vkCommandPool, 1, &_encoderData.vkCommandBuffer);
    }

    _queue->release();
}

/* ICommandEncoder */
IComputePass* VulkanCommandEncoder::beginComputePass(ComputePassDescriptor const* descriptor) noexcept {
    VulkanCommandPassData passData = VulkanCommandPassData(_encoderData);

    try {
        return new VulkanComputePass(this, passData);
    } catch (std::runtime_error err) {
        return nullptr;
    }
}

IRenderPass* VulkanCommandEncoder::beginRenderPass(RenderPassDescriptor const* descriptor) noexcept {
    return nullptr;
}

void VulkanCommandEncoder::insertDebugLabel(const char* label) noexcept {
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

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
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

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
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

    if (_encoderData.functionPointers.debugUtilsEXT.vkCmdEndDebugUtilsLabelEXT == nullptr) {
        return;
    }

    _encoderData.functionPointers.debugUtilsEXT.vkCmdEndDebugUtilsLabelEXT(_encoderData.vkCommandBuffer);
}

void VulkanCommandEncoder::copyBufferToBuffer(IBuffer* dstBuffer, IBuffer* srcBuffer, BufferCopy const* copy) noexcept {
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

    if (dstBuffer->handleType() != ObjectType::Buffer) {
        /* TODO: error */
        return;
    }

    if (srcBuffer->handleType() != ObjectType::Buffer) {
        /* TODO: error */
        return;
    }

    /* TODO: more error checking */

    VkBuffer vkDstBuffer = dstBuffer->handle<VkBuffer>();
    VkBuffer vkSrcBuffer = srcBuffer->handle<VkBuffer>();

    VkBufferCopy vkCopy = {};
    vkCopy.srcOffset = static_cast<VkDeviceSize>(copy->srcOffset);
    vkCopy.dstOffset = static_cast<VkDeviceSize>(copy->dstOffset);
    vkCopy.size = static_cast<VkDeviceSize>(copy->size);

    /* TODO: possibly cache copy commands and pool them together between barriers? */
    _encoderData.functionPointers.commandBuffer10.vkCmdCopyBuffer(_encoderData.vkCommandBuffer, vkSrcBuffer, vkDstBuffer, 1, &vkCopy);
}

void VulkanCommandEncoder::copyBufferToTexture(ITexture* dstTexture, IBuffer* srcBuffer, BufferTextureCopy const* copy) noexcept {
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

    if (dstTexture->handleType() != ObjectType::Image) {
        /* TODO: error */
        return;
    }

    if (srcBuffer->handleType() != ObjectType::Buffer) {
        /* TODO: error */
        return;
    }

    /* TODO: more error checking */

    VkImage vkDstImage = dstTexture->handle<VkImage>();
    VkBuffer vkSrcBuffer = srcBuffer->handle<VkBuffer>();

    VkBufferImageCopy vkCopy = {};
    vkCopy.bufferOffset = static_cast<VkDeviceSize>(copy->bufferOffset);
    vkCopy.bufferRowLength = copy->bufferPitch;
    vkCopy.bufferImageHeight = copy->bufferRows;
    vkCopy.imageSubresource.aspectMask = castEnum<VkImageAspectFlags>(copy->textureAspectFlags);
    vkCopy.imageSubresource.mipLevel = copy->texturePosition.subresource.mip;
    vkCopy.imageSubresource.baseArrayLayer = copy->texturePosition.subresource.layer;
    vkCopy.imageSubresource.layerCount = copy->textureDimensions.subresource.layers;
    vkCopy.imageOffset.x = copy->texturePosition.xyz.x;
    vkCopy.imageOffset.y = copy->texturePosition.xyz.y;
    vkCopy.imageOffset.z = copy->texturePosition.xyz.z;
    vkCopy.imageExtent.width = copy->textureDimensions.extent.width;
    vkCopy.imageExtent.height = copy->textureDimensions.extent.height;
    vkCopy.imageExtent.depth = copy->textureDimensions.extent.depth;

    /* TODO: possibly cache copy commands and pool them together between barriers? */
    _encoderData.functionPointers.commandBuffer10.vkCmdCopyBufferToImage(_encoderData.vkCommandBuffer, vkSrcBuffer, vkDstImage, castEnum<VkImageLayout>(copy->textureLayout), 1, &vkCopy);
}

void VulkanCommandEncoder::copyTextureToTexture(ITexture* dstTexture, ITexture* srcTexture, TextureCopy const* copy) noexcept {
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

    if (dstTexture->handleType() != ObjectType::Image) {
        /* TODO: error */
        return;
    }

    if (srcTexture->handleType() != ObjectType::Image) {
        /* TODO: error */
        return;
    }

    /* TODO: more error checking */

    VkImage vkDstImage = dstTexture->handle<VkImage>();
    VkImage vkSrcImage = srcTexture->handle<VkImage>();

    VkImageCopy vkCopy = {};
    vkCopy.srcSubresource.aspectMask = castEnum<VkImageAspectFlags>(copy->srcAspectFlags);
    vkCopy.srcSubresource.mipLevel = copy->srcPosition.subresource.mip;
    vkCopy.srcSubresource.baseArrayLayer = copy->srcPosition.subresource.layer;
    vkCopy.srcSubresource.layerCount = copy->srcSubresourceDimensions.layers;
    vkCopy.srcOffset.x = copy->srcPosition.xyz.x;
    vkCopy.srcOffset.y = copy->srcPosition.xyz.y;
    vkCopy.srcOffset.z = copy->srcPosition.xyz.z;
    vkCopy.dstSubresource.aspectMask = castEnum<VkImageAspectFlags>(copy->dstAspectFlags);
    vkCopy.dstSubresource.mipLevel = copy->dstPosition.subresource.mip;
    vkCopy.dstSubresource.baseArrayLayer = copy->dstPosition.subresource.layer;
    vkCopy.dstSubresource.layerCount = copy->dstSubresourceDimensions.layers;
    vkCopy.dstOffset.x = copy->dstPosition.xyz.x;
    vkCopy.dstOffset.y = copy->dstPosition.xyz.y;
    vkCopy.dstOffset.z = copy->dstPosition.xyz.z;
    vkCopy.extent.width = copy->extent.width;
    vkCopy.extent.height = copy->extent.height;
    vkCopy.extent.depth = copy->extent.depth;

    /* TODO: possibly cache copy commands and pool them together between barriers? */
    _encoderData.functionPointers.commandBuffer10.vkCmdCopyImage(_encoderData.vkCommandBuffer, vkSrcImage, castEnum<VkImageLayout>(copy->srcLayout), vkDstImage, castEnum<VkImageLayout>(copy->dstLayout), 1, &vkCopy);
}

void VulkanCommandEncoder::copyTextureToBuffer(IBuffer* dstBuffer, ITexture* srcTexture, BufferTextureCopy const* copy) noexcept {
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

    if (dstBuffer->handleType() != ObjectType::Buffer) {
        /* TODO: error */
        return;
    }

    if (srcTexture->handleType() != ObjectType::Image) {
        /* TODO: error */
        return;
    }

    /* TODO: more error checking */

    VkBuffer vkDstBuffer = dstBuffer->handle<VkBuffer>();
    VkImage vkSrcImage = srcTexture->handle<VkImage>();

    VkBufferImageCopy vkCopy = {};
    vkCopy.bufferOffset = static_cast<VkDeviceSize>(copy->bufferOffset);
    vkCopy.bufferRowLength = copy->bufferPitch;
    vkCopy.bufferImageHeight = copy->bufferRows;
    vkCopy.imageSubresource.aspectMask = castEnum<VkImageAspectFlags>(copy->textureAspectFlags);
    vkCopy.imageSubresource.mipLevel = copy->texturePosition.subresource.mip;
    vkCopy.imageSubresource.baseArrayLayer = copy->texturePosition.subresource.layer;
    vkCopy.imageSubresource.layerCount = copy->textureDimensions.subresource.layers;
    vkCopy.imageOffset.x = copy->texturePosition.xyz.x;
    vkCopy.imageOffset.y = copy->texturePosition.xyz.y;
    vkCopy.imageOffset.z = copy->texturePosition.xyz.z;
    vkCopy.imageExtent.width = copy->textureDimensions.extent.width;
    vkCopy.imageExtent.height = copy->textureDimensions.extent.height;
    vkCopy.imageExtent.depth = copy->textureDimensions.extent.depth;

    /* TODO: possibly cache copy commands and pool them together between barriers? */
    _encoderData.functionPointers.commandBuffer10.vkCmdCopyImageToBuffer(_encoderData.vkCommandBuffer, vkSrcImage, castEnum<VkImageLayout>(copy->textureLayout), vkDstBuffer, 1, &vkCopy);
}

void VulkanCommandEncoder::smallBufferUpload(IBuffer* dstBuffer, SmallBufferUpload const* upload) noexcept {
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

    if (upload->size > 65536) {
        /* TODO: error */
        return;
    }

    if (dstBuffer->handleType() != ObjectType::Buffer) {
        /* TODO: error */
        return;
    }

    /* TODO: more error checking */

    VkBuffer vkDstBuffer = dstBuffer->handle<VkBuffer>();
    _encoderData.functionPointers.commandBuffer10.vkCmdUpdateBuffer(_encoderData.vkCommandBuffer, vkDstBuffer, upload->dstOffset, upload->size, reinterpret_cast<void const*>(upload->data));
}

void VulkanCommandEncoder::fillBuffer(IBuffer* dstBuffer, BufferFill const* fill) noexcept {
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

    if (dstBuffer->handleType() != ObjectType::Buffer) {
        /* TODO: error */
        return;
    }

    /* TODO: more error checking */

    VkBuffer vkDstBuffer = dstBuffer->handle<VkBuffer>();
    _encoderData.functionPointers.commandBuffer10.vkCmdFillBuffer(_encoderData.vkCommandBuffer, vkDstBuffer, fill->dstOffset, fill->size, fill->word);
}

void VulkanCommandEncoder::blitTexture(ITexture* dstTexture, ITexture* srcTexture, TextureBlit const* blit) noexcept {
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

    if (dstTexture->handleType() != ObjectType::Image) {
        /* TODO: error */
        return;
    }

    if (srcTexture->handleType() != ObjectType::Image) {
        /* TODO: error */
        return;
    }

    /* TODO: more error checking */

    VkImage vkDstImage = dstTexture->handle<VkImage>();
    VkImage vkSrcImage = srcTexture->handle<VkImage>();

    VkImageBlit vkBlit = {};
    vkBlit.srcSubresource.aspectMask = castEnum<VkImageAspectFlags>(blit->aspectFlags);
    vkBlit.srcSubresource.mipLevel = blit->srcPositions[0].subresource.mip;
    vkBlit.srcSubresource.baseArrayLayer = std::min(blit->srcPositions[0].subresource.layer, blit->srcPositions[1].subresource.layer);
    vkBlit.srcSubresource.layerCount = std::abs(static_cast<int32_t>(blit->srcPositions[1].subresource.layer) - static_cast<int32_t>(blit->srcPositions[0].subresource.layer)) + 1;
    vkBlit.srcOffsets[0].x = blit->srcPositions[0].xyz.x;
    vkBlit.srcOffsets[0].y = blit->srcPositions[0].xyz.y;
    vkBlit.srcOffsets[0].z = blit->srcPositions[0].xyz.z;
    vkBlit.srcOffsets[1].x = blit->srcPositions[1].xyz.x;
    vkBlit.srcOffsets[1].y = blit->srcPositions[1].xyz.y;
    vkBlit.srcOffsets[1].z = blit->srcPositions[1].xyz.z;
    vkBlit.dstSubresource.aspectMask = castEnum<VkImageAspectFlags>(blit->aspectFlags);
    vkBlit.dstSubresource.mipLevel = blit->dstPositions[0].subresource.mip;
    vkBlit.dstSubresource.baseArrayLayer = std::min(blit->dstPositions[0].subresource.layer, blit->dstPositions[1].subresource.layer);
    vkBlit.dstSubresource.layerCount = std::abs(static_cast<int32_t>(blit->dstPositions[1].subresource.layer) - static_cast<int32_t>(blit->dstPositions[0].subresource.layer)) + 1;
    vkBlit.dstOffsets[0].x = blit->dstPositions[0].xyz.x;
    vkBlit.dstOffsets[0].y = blit->dstPositions[0].xyz.y;
    vkBlit.dstOffsets[0].z = blit->dstPositions[0].xyz.z;
    vkBlit.dstOffsets[1].x = blit->dstPositions[1].xyz.x;
    vkBlit.dstOffsets[1].y = blit->dstPositions[1].xyz.y;
    vkBlit.dstOffsets[1].z = blit->dstPositions[1].xyz.z;

    /* TODO: possibly cache blit commands and pool them together between barriers? */
    _encoderData.functionPointers.commandBuffer10.vkCmdBlitImage(_encoderData.vkCommandBuffer, vkSrcImage, castEnum<VkImageLayout>(blit->srcLayout), vkDstImage, castEnum<VkImageLayout>(blit->dstLayout), 1, &vkBlit, castEnum<VkFilter>(blit->filter));
}

void VulkanCommandEncoder::resolveTexture(ITexture* dstTexture, ITexture* srcTexture, TextureResolve const* resolve) noexcept {
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

    if (dstTexture->handleType() != ObjectType::Image) {
        /* TODO: error */
        return;
    }

    if (srcTexture->handleType() != ObjectType::Image) {
        /* TODO: error */
        return;
    }

    /* TODO: more error checking */

    VkImage vkDstImage = dstTexture->handle<VkImage>();
    VkImage vkSrcImage = srcTexture->handle<VkImage>();

    /* TODO: VK_KHR_copy_commands/1.3 support with VkImageResolve2 and vkCmdResolveImage2 to support depth/stencil blits */
    VkImageResolve vkResolve = {};
    vkResolve.srcSubresource.aspectMask = castEnum<VkImageAspectFlags>(resolve->srcAspectFlags);
    vkResolve.srcSubresource.mipLevel = resolve->srcPosition.subresource.mip;
    vkResolve.srcSubresource.baseArrayLayer = resolve->srcPosition.subresource.layer;
    vkResolve.srcSubresource.layerCount = resolve->dimensions.subresource.layers;
    vkResolve.srcOffset.x = resolve->srcPosition.xyz.x;
    vkResolve.srcOffset.y = resolve->srcPosition.xyz.y;
    vkResolve.srcOffset.z = resolve->srcPosition.xyz.z;
    vkResolve.dstSubresource.aspectMask = castEnum<VkImageAspectFlags>(resolve->dstAspectFlags);
    vkResolve.dstSubresource.mipLevel = resolve->dstPosition.subresource.mip;
    vkResolve.dstSubresource.baseArrayLayer = resolve->dstPosition.subresource.layer;
    vkResolve.dstSubresource.layerCount = resolve->dimensions.subresource.layers;
    vkResolve.dstOffset.x = resolve->dstPosition.xyz.x;
    vkResolve.dstOffset.y = resolve->dstPosition.xyz.y;
    vkResolve.dstOffset.z = resolve->dstPosition.xyz.z;
    vkResolve.extent.width = resolve->dimensions.extent.width;
    vkResolve.extent.height = resolve->dimensions.extent.height;
    vkResolve.extent.depth = resolve->dimensions.extent.depth;

    /* TODO: possibly cache resolve commands and pool them together between barriers? */
    _encoderData.functionPointers.commandBuffer10.vkCmdResolveImage(_encoderData.vkCommandBuffer, vkSrcImage, castEnum<VkImageLayout>(resolve->srcLayout), vkDstImage, castEnum<VkImageLayout>(resolve->dstLayout), 1, &vkResolve);
}

void VulkanCommandEncoder::clearColorTexture(ITexture* texture, ColorTextureClear const* clear) noexcept {
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

    if (texture->handleType() != ObjectType::Image) {
        /* TODO: error */
        return;
    }

    /* TODO: more error checking */

    VkImage vkImage = texture->handle<VkImage>();

    VkClearColorValue vkColor = {};
    vkColor.float32[0] = clear->color[0];
    vkColor.float32[1] = clear->color[1];
    vkColor.float32[2] = clear->color[2];
    vkColor.float32[3] = clear->color[3];

    VkImageSubresourceRange vkSubresourceRange = {};
    vkSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    vkSubresourceRange.baseMipLevel = clear->subresourcePosition.mip;
    vkSubresourceRange.levelCount = clear->subresourceDimensions.mips;
    vkSubresourceRange.baseArrayLayer = clear->subresourcePosition.layer;
    vkSubresourceRange.layerCount = clear->subresourceDimensions.layers;

    /* TODO: possibly cache clear commands and pool them together between barriers/passes? */
    _encoderData.functionPointers.commandBuffer10.vkCmdClearColorImage(_encoderData.vkCommandBuffer, vkImage, castEnum<VkImageLayout>(clear->layout), &vkColor, 1, &vkSubresourceRange);
}

void VulkanCommandEncoder::clearDepthStencilTexture(ITexture* texture, DepthStencilTextureClear const* clear) noexcept {
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

    if (texture->handleType() != ObjectType::Image) {
        /* TODO: error */
        return;
    }

    /* TODO: more error checking */

    VkImage vkImage = texture->handle<VkImage>();

    VkClearDepthStencilValue vkDepthStencil = {};
    vkDepthStencil.depth = clear->depth;
    vkDepthStencil.stencil = clear->stencil;

    VkImageSubresourceRange vkSubresourceRange = {};
    vkSubresourceRange.aspectMask = castEnum<VkImageAspectFlags>(clear->aspectFlags);
    vkSubresourceRange.baseMipLevel = clear->subresourcePosition.mip;
    vkSubresourceRange.levelCount = clear->subresourceDimensions.mips;
    vkSubresourceRange.baseArrayLayer = clear->subresourcePosition.layer;
    vkSubresourceRange.layerCount = clear->subresourceDimensions.layers;

    /* TODO: possibly cache clear commands and pool them together between barriers/passes? */
    _encoderData.functionPointers.commandBuffer10.vkCmdClearDepthStencilImage(_encoderData.vkCommandBuffer, vkImage, castEnum<VkImageLayout>(clear->layout), &vkDepthStencil, 1, &vkSubresourceRange);
}

void VulkanCommandEncoder::globalMemoryBarrier(GeneralBarrier const* barrier) noexcept {
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

    VkMemoryBarrier vkBarrier = {};
    vkBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    vkBarrier.srcAccessMask = castEnum<VkAccessFlags>(barrier->srcAccess);
    vkBarrier.dstAccessMask = castEnum<VkAccessFlags>(barrier->dstAccess);

    /* TODO: use vkCmdPipelineBarrier2 when available */
    /* TODO: cache barriers and pool them together */
    _encoderData.functionPointers.commandBuffer10.vkCmdPipelineBarrier(_encoderData.vkCommandBuffer,
        castEnum<VkPipelineStageFlags>(barrier->srcStage), castEnum<VkPipelineStageFlags>(barrier->dstStage), 0,
        1, &vkBarrier,
        0, nullptr,
        0, nullptr
    );
}

void VulkanCommandEncoder::transitionTexture(ITexture* texture, TextureTransition const* transition) noexcept {
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

    if (texture->handleType() != ObjectType::Image) {
        /* TODO: error */
        return;
    }

    /* TODO: more error checking */

    VkImage vkImage = texture->handle<VkImage>();

    VkImageMemoryBarrier vkTransition = {};
    vkTransition.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    vkTransition.srcAccessMask = castEnum<VkAccessFlags>(transition->general.srcAccess);
    vkTransition.dstAccessMask = castEnum<VkAccessFlags>(transition->general.dstAccess);
    vkTransition.oldLayout = castEnum<VkImageLayout>(transition->oldLayout);
    vkTransition.newLayout = castEnum<VkImageLayout>(transition->newLayout);
    vkTransition.srcQueueFamilyIndex = transition->transfer.oldFamily;
    vkTransition.dstQueueFamilyIndex = transition->transfer.newFamily;
    vkTransition.image = vkImage;
    vkTransition.subresourceRange.aspectMask = castEnum<VkImageAspectFlags>(transition->aspectFlags);
    vkTransition.subresourceRange.baseArrayLayer = transition->subresourcePosition.layer;
    vkTransition.subresourceRange.layerCount = transition->subresourceDimensions.layers;
    vkTransition.subresourceRange.baseMipLevel = transition->subresourcePosition.mip;
    vkTransition.subresourceRange.levelCount = transition->subresourceDimensions.mips;

    /* TODO: use vkCmdPipelineBarrier2 when available */
    /* TODO: cache barriers and pool them together */
    _encoderData.functionPointers.commandBuffer10.vkCmdPipelineBarrier(_encoderData.vkCommandBuffer,
        castEnum<VkPipelineStageFlags>(transition->general.srcStage), castEnum<VkPipelineStageFlags>(transition->general.dstStage), 0,
        0, nullptr,
        0, nullptr,
        1, &vkTransition
    );
}

void VulkanCommandEncoder::transitionBuffer(IBuffer* buffer, BufferTransition const* transition) noexcept {
    if (_ended) {
        /* TODO: encoder already ended: can't encode anymore commands */
        return;
    }

    if (buffer->handleType() != ObjectType::Buffer) {
        /* TODO: error */
        return;
    }

    /* TODO: more error checking */

    VkBuffer vkBuffer = buffer->handle<VkBuffer>();

    VkBufferMemoryBarrier vkTransition = {};
    vkTransition.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    vkTransition.srcAccessMask = castEnum<VkAccessFlags>(transition->general.srcAccess);
    vkTransition.dstAccessMask = castEnum<VkAccessFlags>(transition->general.dstAccess);
    vkTransition.srcQueueFamilyIndex = transition->transfer.oldFamily;
    vkTransition.dstQueueFamilyIndex = transition->transfer.newFamily;
    vkTransition.buffer = vkBuffer;
    vkTransition.offset = transition->offset;
    vkTransition.size = transition->size;

    /* TODO: use vkCmdPipelineBarrier2 when available */
    /* TODO: cache barriers and pool them together */
    _encoderData.functionPointers.commandBuffer10.vkCmdPipelineBarrier(_encoderData.vkCommandBuffer,
        castEnum<VkPipelineStageFlags>(transition->general.srcStage), castEnum<VkPipelineStageFlags>(transition->general.dstStage), 0,
        0, nullptr,
        1, &vkTransition,
        0, nullptr
    );
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

void const* VulkanCommandEncoder::vkData() const noexcept {
    return &_encoderData;
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
