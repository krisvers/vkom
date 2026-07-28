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
class IComputeEncoder;
class IRenderEncoder;

class ICommandEncoder : public INullable, public IHandled, public ICollected, public IParent, public IChild, public IDispatchable {
public:
    /* passes */
    virtual Result beginComputePass(ComputePassDescriptor const* descriptor, IComputeEncoder** encoder) noexcept = 0;
    virtual Result beginRenderPass(RenderPassDescriptor const* descriptor, IRenderEncoder** encoder) noexcept = 0;

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

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

}