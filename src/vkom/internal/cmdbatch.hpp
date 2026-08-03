#pragma once

#include <vector>

#include <vkom/enums.hpp>
#include <vkom/cmdbatch.hpp>

#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vulkan.hpp>

namespace vkom {

namespace internal {

struct VulkanCommandBatchFunctionPointers {
    CommandBufferFunctionPointers10 commandBuffer10;
    CommandBufferFunctionPointersDebugUtilsEXT debugUtilsEXT;
};

class VulkanInstance;
class VulkanAdapter;
class VulkanDevice;
class VulkanQueue;
class VulkanCommandEncoder;

class VulkanCommandBatch final : public ICommandBatch {
private:
    bool _debug = false;
    bool _inheritedHandle = false;
    VulkanQueue* _queue = nullptr;
    VulkanDevice* _device = nullptr;
    VulkanAdapter* _adapter = nullptr;
    VulkanInstance* _instance = nullptr;
    VkCommandBuffer _vkCommandBuffer = nullptr;
    VkAllocationCallbacks const* _vkAllocationCallbacks = nullptr;
    VulkanCommandBatchFunctionPointers _functionPointers = {};

    bool _ended = false;

public:
    VulkanCommandBatch(bool debug, bool inheritedHandle, VulkanQueue* queue, VkCommandBuffer vkCommandBuffer, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanCommandBatchFunctionPointers const& functionPointers);
    ~VulkanCommandBatch();

    /* ICommandBatch */
    Result record(ICommandEncoder* encoder) noexcept override;
    Result submit(CommandBatchSubmitInfo const* submitInfo) noexcept override;
    void discard() noexcept override;

    /* INullable */
    bool isNull() const noexcept override;

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
