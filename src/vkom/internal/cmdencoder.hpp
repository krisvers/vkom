#pragma once

#include <vkom/enums.hpp>
#include <vkom/cmdencoder.hpp>

#include <vkom/queue.hpp>
#include <vkom/device.hpp>
#include <vkom/adapter.hpp>
#include <vkom/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

class VulkanCommandEncoder final : virtual public ICommandEncoder, virtual public ParentByVector, virtual public CollectedByHeap {
private:
    bool _inheritedHandle = false;
    IQueue* _queue = nullptr;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanCommandEncoderData _encoderData;

    IRenderPass* _renderPass = nullptr;
    bool _ended = false;

    void hashLabelToColor(const char* label, float& r, float& g, float& b);

    friend class VulkanCommandBatch;

public:
    VulkanCommandEncoder(bool inheritedHandle, IQueue* queue, VulkanCommandEncoderData const& encoderData);
    ~VulkanCommandEncoder();

    /* ICommandEncoder */
    IComputePass* beginComputePass(ComputePassDescriptor const* descriptor) noexcept override;
    IRenderPass* beginRenderPass(RenderPassDescriptor const* descriptor) noexcept override;

    void insertDebugLabel(const char* label) noexcept override;
    void pushDebugGroup(const char* label) noexcept override;
    void popDebugGroup() noexcept override;

    void copyBufferToBuffer(IBuffer* dstBuffer, IBuffer* srcBuffer, BufferCopy const* copy) noexcept override;
    void copyBufferToTexture(ITexture* dstTexture, IBuffer* srcBuffer, BufferTextureCopy const* copy) noexcept override;
    void copyTextureToTexture(ITexture* dstTexture, ITexture* srcTexture, TextureCopy const* copy) noexcept override;
    void copyTextureToBuffer(IBuffer* dstBuffer, ITexture* srcTexture, BufferTextureCopy const* copy) noexcept override;
    void smallBufferUpload(IBuffer* dstBuffer, SmallBufferUpload const* upload) noexcept override;
    void fillBuffer(IBuffer* dstBuffer, BufferFill const* fill) noexcept override;

    void blitTexture(ITexture* dstTexture, ITexture* srcTexture, TextureBlit const* blit) noexcept override;
    void resolveTexture(ITexture* dstTexture, ITexture* srcTexture, TextureResolve const* resolve) noexcept override;
    void clearColorTexture(ITexture* texture, ColorTextureClear const* clear) noexcept override;
    void clearDepthStencilTexture(ITexture* texture, DepthStencilTextureClear const* clear) noexcept override;

    void globalMemoryBarrier(GeneralBarrier const* barrier) noexcept override;
    void transitionTexture(ITexture* texture, TextureTransition const* transition) noexcept override;
    void transitionBuffer(IBuffer* buffer, BufferTransition const* transition) noexcept override;

    Result finish() noexcept override;
    Result batch(ICommandBatch** batch) noexcept override;

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    void const* vkData() const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IDispatchable */
    void* loadDispatchSymbol(const char* symbol) override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

}

}
