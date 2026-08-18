#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

#include <vkom/cmdpasses.hpp>
#include <vkom/resource.hpp>
#include <vkom/buffer.hpp>
#include <vkom/texture.hpp>

namespace vkom {

struct BufferCopy {
    uint64_t srcOffset;
    uint64_t dstOffset;
    uint64_t size;
};

struct TextureCopy {
    TextureAspectFlags srcAspectFlags;
    TextureLayout srcLayout;
    TexturePosition srcPosition;
    TextureSubresourceDimensions srcSubresourceDimensions;
    TextureLayout dstLayout;
    TextureAspectFlags dstAspectFlags;
    TexturePosition dstPosition;
    TextureSubresourceDimensions dstSubresourceDimensions;
    TextureExtent extent;
};

struct BufferTextureCopy {
    uint64_t bufferOffset;
    uint32_t bufferPitch;
    uint32_t bufferRows;
    TextureLayout textureLayout;
    TextureAspectFlags textureAspectFlags;
    TexturePosition texturePosition;
    TextureDimensions textureDimensions;
};

struct SmallBufferUpload {
    uint64_t dstOffset;
    uint64_t size;
    uint32_t const* data;
};

struct BufferFill {
    uint64_t dstOffset;
    uint64_t size;
    uint32_t word;
};

struct TextureBlit {
    TextureLayout srcLayout;
    TexturePosition srcPositions[2];
    TextureLayout dstLayout;
    TexturePosition dstPositions[2];
    TextureAspectFlags aspectFlags;
    TexelFilter filter;
};

struct TextureResolve {
    TextureLayout srcLayout;
    TextureAspectFlags srcAspectFlags;
    TexturePosition srcPosition;
    TextureLayout dstLayout;
    TextureAspectFlags dstAspectFlags;
    TexturePosition dstPosition;
    TextureDimensions dimensions;
};

struct ColorTextureClear {
    TextureLayout layout;
    float color[4];
    TextureSubresourcePosition subresourceOffset;
    TextureSubresourceDimensions subresourceRange;
};

struct DepthStencilTextureClear {
    TextureLayout layout;
    float depth;
    uint32_t stencil;
    TextureAspectFlags aspectFlags;
    TextureSubresourcePosition subresourceOffset;
    TextureSubresourceDimensions subresourceRange;
};

struct GeneralBarrier {
    PipelineStageFlags srcStage;
    PipelineStageFlags dstStage;
    ResourceAccessFlags srcAccess;
    ResourceAccessFlags dstAccess;
};

struct ResourceQueueFamilyTransfer {
    uint32_t oldFamily;
    uint32_t newFamily;
};

struct TextureTransition {
    GeneralBarrier general;
    ResourceQueueFamilyTransfer transfer;
    TextureLayout oldLayout;
    TextureLayout newLayout;
    TextureAspectFlags aspectFlags;
    TextureSubresourcePosition subresourcePosition;
    TextureSubresourceDimensions subresourceDimensions;
};

struct BufferTransition {
    GeneralBarrier general;
    ResourceQueueFamilyTransfer transfer;
    uint64_t offset;
    uint64_t size;
};

class ICommandBatch;

class ICommandEncoder : virtual public IHandled, virtual public ICollected, virtual public IParent, virtual public IChild, virtual public IDispatchable {
public:
    /* passes */
    virtual IComputePass* beginComputePass(ComputePassDescriptor const* descriptor) noexcept = 0;
    virtual IRenderPass* beginRenderPass(RenderPassDescriptor const* descriptor) noexcept = 0;

    /* debug utilities */
    virtual void insertDebugLabel(const char* label) noexcept = 0;
    virtual void pushDebugGroup(const char* label) noexcept = 0;
    virtual void popDebugGroup() noexcept = 0;

    /* transfer commands */
    virtual void copyBufferToBuffer(IBuffer* dstBuffer, IBuffer* srcBuffer, BufferCopy const* copy) noexcept = 0;
    virtual void copyBufferToTexture(ITexture* dstTexture, IBuffer* srcBuffer, BufferTextureCopy const* copy) noexcept = 0;
    virtual void copyTextureToTexture(ITexture* dstTexture, ITexture* srcTexture, TextureCopy const* copy) noexcept = 0;
    virtual void copyTextureToBuffer(IBuffer* dstBuffer, ITexture* srcTexture, BufferTextureCopy const* copy) noexcept = 0;
    virtual void smallBufferUpload(IBuffer* dstBuffer, SmallBufferUpload const* upload) noexcept = 0;
    virtual void fillBuffer(IBuffer* dstBuffer, BufferFill const* fill) noexcept = 0;

    /* texture modification commands */
    virtual void blitTexture(ITexture* dstTexture, ITexture* srcTexture, TextureBlit const* blit) noexcept = 0;
    virtual void resolveTexture(ITexture* dstTexture, ITexture* srcTexture, TextureResolve const* resolve) noexcept = 0;
    virtual void clearColorTexture(ITexture* texture, ColorTextureClear const* clear) noexcept = 0;
    virtual void clearDepthStencilTexture(ITexture* texture, DepthStencilTextureClear const* clear) noexcept = 0;

    /* synchronization commands */
    virtual void globalMemoryBarrier(GeneralBarrier const* barrier) noexcept = 0;
    virtual void transitionTexture(ITexture* texture, TextureTransition const* transition) noexcept = 0;
    virtual void transitionBuffer(IBuffer* buffer, BufferTransition const* transition) noexcept = 0;

    /* once finish is called, no more commands can be encoded and no more passes can begin */
    virtual Result finish() noexcept = 0;

    /* equivalent to ICommandEncoder::finish + IQueue::acquireCommandBatch + ICommandBatch::record */
    virtual Result batch(ICommandBatch** batch) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("2039d9ec-fa70-42c3-b337-08fa2ee6e38a");
        return iid;
    }
};

}
