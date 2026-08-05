#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>
#include <vkom/cmdpasses.hpp>
#include <vkom/resource.hpp>
#include <vkom/buffer.hpp>
#include <vkom/texture.hpp>

namespace vkom {

struct GeneralBarrier {
    PipelineStageFlags srcStage;
    PipelineStageFlags dstStage;
    ResourceAccessFlags srcAccess;
    ResourceAccessFlags dstAccess;
};

struct TextureTransition {
    GeneralBarrier general;
    TextureLayout oldLayout;
    TextureLayout newLayout;
    TexturePosition base;
    TextureDimensions range;
    ITexture* texture;
};

struct BufferTransition {
    GeneralBarrier general;
    uint64_t offset;
    uint64_t size;
    IBuffer* buffer;
};

struct ResourceQueueFamilyTransfer {
    uint32_t oldFamily;
    uint32_t newFamily;
    IResource* resource;
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
    virtual void copyBufferToBuffer() noexcept = 0;
    virtual void copyBufferToImage() noexcept = 0;
    virtual void copyImageToImage() noexcept = 0;
    virtual void copyImageToBuffer() noexcept = 0;
    virtual void smallBufferUpload() noexcept = 0;
    virtual void fillBuffer() noexcept = 0;

    /* synchronization commands */
    virtual void globalMemoryBarrier(GeneralBarrier const* barrier) noexcept = 0;
    virtual void transitionTexture(TextureTransition const* transition) noexcept = 0;
    virtual void transitionBuffer(BufferTransition const* transition) noexcept = 0;
    virtual void transferResourceQueueFamily(ResourceQueueFamilyTransfer const* transfer) noexcept = 0;

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
