#pragma once

#include <vector>

#include <vkom/enums.hpp>
#include <vkom/queue.hpp>

#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vulkan.hpp>

namespace vkom {

namespace internal {

class VulkanInstance;
class VulkanAdapter;
class VulkanDevice;

class VulkanQueue final : public IQueue {
private:
    bool _debug = false;
    bool _inheritedHandle = false;
    VulkanDevice* _device = nullptr;
    VulkanAdapter* _adapter = nullptr;
    VulkanInstance* _instance = nullptr;
    uint32_t _family = 0;
    uint32_t _index = 0;
    QueueFlags _flags = QueueFlags::None;
    VkQueue _vkQueue = nullptr;
    VkAllocationCallbacks const* _vkAllocationCallbacks = nullptr;
    VulkanQueueFunctionPointers _functionPointers = {};

    /* TODO: possibly expand to multiple pools? */
    VkCommandPool _vkCommandPool = nullptr;

    /* ICollected */
    uint32_t _referenceCount = 1;

    /* IParent */
    std::vector<IChild*> _children = {};

    VkCommandBuffer acquireCommandBuffer(bool secondary);
    void releaseCommandBuffer(VkCommandBuffer vkCommandBuffer);

    friend class VulkanCommandEncoder;
    friend class VulkanCommandBatch;

public:
    VulkanQueue(bool debug, bool inheritedHandle, VulkanDevice* device, uint32_t family, uint32_t index, QueueFlags flags, VkQueue vkQueue, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanQueueFunctionPointers const& functionPointers);
    ~VulkanQueue();

    /* IQueue */
    uint32_t family() const noexcept override;
    uint32_t index() const noexcept override;
    QueueFlags flags() const noexcept override;

    Result waitIdle() const noexcept override;

    Result acquireCommandEncoder(ICommandEncoder** encoder) noexcept override;
    Result acquireCommandBatch(ICommandBatch** batch) noexcept override;

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
