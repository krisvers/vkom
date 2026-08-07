#pragma once

#include <vector>

#include <vkom/enums.hpp>
#include <vkom/device.hpp>

#include <vkom/heap.hpp>
#include <vkom/queue.hpp>
#include <vkom/adapter.hpp>
#include <vkom/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/vma.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

struct VulkanDeviceQueueFamily {
    QueueFlags flags;
    VkQueueFamilyProperties properties;
    std::vector<IQueue*> queues;
};

class VulkanDevice final : virtual public IDevice, virtual public ParentByVector, virtual public CollectedByHeap {
private:
    bool _inheritedHandle = false;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanDeviceData _deviceData;
    std::vector<const char*> _enabledExtensions = {};

    std::vector<VulkanDeviceQueueFamily> _queueFamilies = {};
    IHeap* _defaultHeap = nullptr;

public:
    VulkanDevice(bool inheritedHandle, IAdapter* adapter, VulkanDeviceData const& deviceData, std::vector<const char*> const& enabledExtensions);
    ~VulkanDevice();

    /* IDevice */
    Result waitIdle() const noexcept override;
    bool queryExtension(const char* extension) const noexcept override;

    Result acquireQueue(uint32_t family, QueueFlags flags, IQueue** queue) noexcept override;

    IHeap* defaultHeap() noexcept override;
    Result createHeap(BufferUsageFlags bufferUsages, TextureUsageFlags textureUsages, MemoryLocationFlags memoryLocation, IHeap** heap) noexcept override;

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
