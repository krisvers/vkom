#pragma once

#include <vector>

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

    /* ICollected */
    uint32_t _referenceCount = 1;

    /* IParent */
    std::vector<IChild*> _children = {};

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

    void copyBufferToBuffer() noexcept override;
    void copyBufferToImage() noexcept override;
    void copyImageToImage() noexcept override;
    void copyImageToBuffer() noexcept override;
    void smallBufferUpload() noexcept override;
    void fillBuffer() noexcept override;

    void globalMemoryBarrier(GeneralBarrier const* barrier) noexcept override;
    void transitionTexture(TextureTransition const* transition) noexcept override;
    void transitionBuffer(BufferTransition const* transition) noexcept override;
    void transferResourceQueueFamily(ResourceQueueFamilyTransfer const* transfer) noexcept override;

    Result finish() noexcept override;
    Result batch(ICommandBatch** batch) noexcept override;

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IDispatchable */
    void* loadDispatchSymbol(const char* symbol) override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

}

}
