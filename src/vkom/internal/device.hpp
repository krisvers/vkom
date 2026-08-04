#pragma once

#include <vector>

#include <vkom/enums.hpp>
#include <vkom/device.hpp>

#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/vma.hpp>

namespace vkom {

namespace internal {

class VulkanInstance;
class VulkanAdapter;
class VulkanQueue;
class VulkanHeap;

struct VulkanDeviceQueueFamily {
    QueueFlags flags;
    VkQueueFamilyProperties properties;
    std::vector<VulkanQueue*> queues;
};

class VulkanDevice final : public IDevice {
private:
    bool _debug = false;
    bool _inheritedHandle = false;
    VulkanAdapter* _adapter = nullptr;
    VulkanInstance* _instance = nullptr;
    VkDevice _vkDevice = nullptr;
    PFN_vkGetDeviceProcAddr _vkGetDeviceProcAddr = nullptr;
    VkAllocationCallbacks const* _vkAllocationCallbacks = nullptr;
    VulkanDeviceFunctionPointers _functionPointers = {};
    std::vector<const char*> _enabledExtensions = {};

    std::vector<VulkanDeviceQueueFamily> _queueFamilies = {};
    VmaAllocator _vmaAllocator = nullptr;
    VulkanHeap* _defaultHeap = nullptr;

    /* ICollected */
    uint32_t _referenceCount = 1;

    /* IParent */
    std::vector<IChild*> _children = {};

public:
    VulkanDevice(bool debug, bool inheritedHandle, VulkanAdapter* adapter, VkDevice vkDevice, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanDeviceFunctionPointers const& functionPointers, std::vector<const char*> const& enabledExtensions);
    ~VulkanDevice();

    /* IDevice */
    Result waitIdle() const noexcept override;
    Result acquireQueue(uint32_t family, QueueFlags flags, IQueue** queue) noexcept override;
    IHeap* defaultHeap() noexcept override;
    Result createHeap(BufferUsageFlags bufferUsages, TextureUsageFlags textureUsages, MemoryLocationFlags memoryLocation, IHeap** heap) noexcept override;

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
