#pragma once

#include <vector>

#include <vkom/enums.hpp>
#include <vkom/queue.hpp>

#include <vkom/device.hpp>
#include <vkom/adapter.hpp>
#include <vkom/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

class VulkanQueue final : virtual public IQueue, virtual public ParentByVector, virtual public CollectedByHeap {
private:
    bool _inheritedHandle = false;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanQueueData _queueData;
    QueueFlags _flags = QueueFlags::None;

    /* TODO: possibly expand to multiple pools? */
    VkCommandPool _vkCommandPool = nullptr;

    VkCommandBuffer acquireCommandBuffer(bool secondary);

public:
    VulkanQueue(bool inheritedHandle, IDevice* device, VulkanQueueData const& queueData);
    ~VulkanQueue();

    /* IQueue */
    uint32_t family() const noexcept override;
    uint32_t index() const noexcept override;
    QueueFlags flags() const noexcept override;

    Result waitIdle() const noexcept override;

    Result acquireCommandEncoder(ICommandEncoder** encoder) noexcept override;
    Result acquireCommandBatch(ICommandBatch** batch) noexcept override;

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
