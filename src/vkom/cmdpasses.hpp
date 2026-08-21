#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

#include <vkom/buffer.hpp>
#include <vkom/texture.hpp>

namespace vkom {

struct ComputePassDescriptor {
    /* TODO: */
};

struct RenderPassDescriptor {
    /* TODO: description */
};

class IDescriptorSet;
class IPipeline;
class IPipelineLayout;

class IPass : virtual public IChild {
public:
    virtual void bindPipeline(IPipeline* pipeline) noexcept = 0;
    virtual void bindDescriptorSet(IPipelineLayout* layout, uint32_t id, IDescriptorSet* descriptorSet, uint32_t dynamicOffsetCount, uint32_t const* dynamicOffsets) noexcept = 0;
    virtual void pushConstants(IPipelineLayout* layout, ShaderStageFlags stages, uint32_t offset, uint32_t size, void const* data) noexcept = 0;
    virtual void end() noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("dc292f09-0fb4-467c-908f-94f41b2b3819");
        return iid;
    }
};

class IComputePass : virtual public IPass {
public:
    virtual void dispatch(uint32_t width, uint32_t height, uint32_t depth) noexcept = 0;
    virtual void dispatchIndirect(IIndirectBuffer* buffer, uint32_t offset) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("c5062949-36c0-470b-bc5f-561caf85cae2");
        return iid;
    }
};

struct ColorTargetClear {
    float color[4];
    TexturePosition offset;
    TextureDimensions range;
};

struct DepthStencilTargetClear {
    float depth;
    uint32_t stencil;
    TextureAspectFlags aspectFlags;
    TexturePosition offset;
    TextureDimensions range;
};

struct Viewport {
    float x;
    float y;
    float width;
    float height;
    float near;
    float far;
};

struct Scissor {
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
};

class IRenderPass : public IPass {
public:
    virtual void nextSubpass() noexcept = 0;

    virtual void clearColorTarget(uint32_t id, ColorTargetClear const* clear) noexcept = 0;
    virtual void clearDepthStencilTarget(DepthStencilTargetClear const* clear) noexcept = 0;

    virtual void setViewport(uint32_t index, Viewport const* viewport) noexcept = 0;
    virtual void setScissor(uint32_t index, Scissor const* scissor) noexcept = 0;

    virtual void setLineWidth(float lineWidth) noexcept = 0;
    virtual void setDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) noexcept = 0;
    virtual void setBlendConstants(float const blendConstants[4]) noexcept = 0;
    virtual void setDepthBounds(float near, float far) noexcept = 0;

    virtual void setStencilCompareMask(StencilFaceFlags faces, uint32_t compareMask) noexcept = 0;
    virtual void setStencilWriteMask(StencilFaceFlags faces, uint32_t writeMask) noexcept = 0;
    virtual void setStencilReference(StencilFaceFlags faces, uint32_t reference) noexcept = 0;

    virtual void bindIndexBuffer(IIndexBuffer* buffer, uint64_t offset, IndexType type) noexcept = 0;
    virtual void bindVertexBuffer(uint32_t binding, IVertexBuffer* buffer, uint64_t offset) noexcept = 0;

    virtual void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) noexcept = 0;
    virtual void drawIndexed(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) noexcept = 0;
    virtual void drawIndirect(IIndirectBuffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) noexcept = 0;
    virtual void drawIndirectIndexed(IIndirectBuffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("87e5dc28-00b3-40d6-a174-8192e7809b68");
        return iid;
    }
};

}
