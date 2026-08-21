#pragma once

#include <vkom/cmdpasses.hpp>

#include <vkom/enums.hpp>
#include <vkom/cmdencoder.hpp>
#include <vkom/queue.hpp>
#include <vkom/device.hpp>
#include <vkom/adapter.hpp>
#include <vkom/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

class VulkanComputePass final : virtual public IComputePass {
private:
    ICommandEncoder* _encoder = nullptr;
    IQueue* _queue = nullptr;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanComputePassData _passData;

public:
    VulkanComputePass(ICommandEncoder* encoder, VulkanComputePassData const& passData);
    ~VulkanComputePass();

    /* IComputePass */
    void dispatch(uint32_t width, uint32_t height, uint32_t depth) noexcept override;
    void dispatchIndirect(IIndirectBuffer* buffer, uint32_t offset) noexcept override;

    /* IPass */
    void bindPipeline(IPipeline* pipeline) noexcept override;
    void bindDescriptorSet(IPipelineLayout* layout, uint32_t id, IDescriptorSet* descriptorSet, uint32_t dynamicOffsetCount, uint32_t const* dynamicOffsets) noexcept override;
    void pushConstants(IPipelineLayout* layout, ShaderStageFlags stages, uint32_t offset, uint32_t size, void const* data) noexcept override;
    void end() noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

class VulkanRenderPass final : virtual public IRenderPass {
private:
    ICommandEncoder* _encoder = nullptr;
    IQueue* _queue = nullptr;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanRenderPassData _passData;

public:
    VulkanRenderPass(ICommandEncoder* encoder, VulkanRenderPassData const& passData);
    ~VulkanRenderPass();

    /* IRenderPass */
    void nextSubpass() noexcept override;

    void clearColorTarget(uint32_t id, ColorTargetClear const* clear) noexcept override;
    void clearDepthStencilTarget(DepthStencilTargetClear const* clear) noexcept override;

    void setViewport(uint32_t index, Viewport const* viewport) noexcept override;
    void setScissor(uint32_t index, Scissor const* scissor) noexcept override;

    void setLineWidth(float lineWidth) noexcept override;
    void setDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) noexcept override;
    void setBlendConstants(float const blendConstants[4]) noexcept override;
    void setDepthBounds(float near, float far) noexcept override;

    void setStencilCompareMask(StencilFaceFlags faces, uint32_t compareMask) noexcept override;
    void setStencilWriteMask(StencilFaceFlags faces, uint32_t writeMask) noexcept override;
    void setStencilReference(StencilFaceFlags faces, uint32_t reference) noexcept override;

    void bindIndexBuffer(IIndexBuffer* buffer, uint64_t offset, IndexType type) noexcept override;
    void bindVertexBuffer(uint32_t binding, IVertexBuffer* buffer, uint64_t offset) noexcept override;

    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) noexcept override;
    void drawIndexed(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) noexcept override;
    void drawIndirect(IIndirectBuffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) noexcept override;
    void drawIndirectIndexed(IIndirectBuffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) noexcept override;

    /* IPass */
    void bindPipeline(IPipeline* pipeline) noexcept override;
    void bindDescriptorSet(IPipelineLayout* layout, uint32_t id, IDescriptorSet* descriptorSet, uint32_t dynamicOffsetCount, uint32_t const* dynamicOffsets) noexcept override;
    void pushConstants(IPipelineLayout* layout, ShaderStageFlags stages, uint32_t offset, uint32_t size, void const* data) noexcept override;
    void end() noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

}

}
