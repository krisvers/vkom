#pragma once

#include <vector>

#include <vkom/enums.hpp>
#include <vkom/cmdencoder.hpp>

#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vulkan.hpp>

namespace vkom {

namespace internal {

struct VulkanCommandEncoderFunctionPointers {
    CommandBufferFunctionPointers10 commandBuffer10;
    CommandBufferFunctionPointersDebugUtilsEXT debugUtilsEXT;
};

class VulkanInstance;
class VulkanAdapter;
class VulkanDevice;
class VulkanQueue;
class VulkanRenderPass;

class VulkanCommandEncoder final : public ICommandEncoder {
private:
    bool _debug = false;
    bool _inheritedHandle = false;
    VulkanQueue* _queue = nullptr;
    VulkanDevice* _device = nullptr;
    VulkanAdapter* _adapter = nullptr;
    VulkanInstance* _instance = nullptr;
    VkCommandBuffer _vkCommandBuffer = nullptr;
    VkAllocationCallbacks const* _vkAllocationCallbacks = nullptr;
    VulkanCommandEncoderFunctionPointers _functionPointers = {};

    VulkanRenderPass* _renderPass = nullptr;
    bool _ended = false;

    /* ICollected */
    uint32_t _referenceCount = 1;

    /* IParent */
    std::vector<IChild*> _children = {};

    void hashLabelToColor(const char* label, float& r, float& g, float& b);

    friend class VulkanCommandBatch;

public:
    VulkanCommandEncoder(bool debug, bool inheritedHandle, VulkanQueue* queue, VkCommandBuffer vkCommandBuffer, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanCommandEncoderFunctionPointers const& functionPointers);
    ~VulkanCommandEncoder();

    /* ICommandEncoder */
    Result beginComputePass(ComputePassDescriptor const* descriptor, IComputePass** pass) noexcept override;
    Result beginRenderPass(RenderPassDescriptor const* descriptor, IRenderPass** pass) noexcept override;

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

    /* INullable */
    bool isNull() const noexcept override;

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    /* ICollected */
    uint32_t release() override;
    uint32_t retain() override;

    /* IParent */
    bool hasChild(IChild const* child) const noexcept override;
    IChild* enumerateChildren(uint32_t id) const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IDispatchable */
    void* loadDispatchSymbol(const char* symbol) override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

}

}
