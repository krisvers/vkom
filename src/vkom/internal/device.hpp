#pragma once

#include <vector>

#include <vkom/enums.hpp>
#include <vkom/device.hpp>

#include <vkom/heap.hpp>
#include <vkom/queue.hpp>
#include <vkom/pipeline.hpp>
#include <vkom/descriptor.hpp>
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

/* NOTE: while this class implements IWSIDevice, instances are not guaranteed to advertise the interface
*   only instances that support swapchains will advertise support
*/

class VulkanDevice final : virtual public IDevice, virtual public IWSIDevice, virtual public ParentByVector, virtual public CollectedByHeap {
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

    /* IWSIDevice */
    Result createSwapchain(ISurface* surface, SwapchainInfo const* info, ISwapchain** swapchain) noexcept override;
    Result createSwapchainAndSurface(SurfaceWSIInfo const* surfaceInfo, SwapchainInfo const* swapchainInfo, ISurface** surface, ISwapchain** swapchain) noexcept override;

    /* IDevice */
    Result waitIdle() const noexcept override;
    bool queryExtension(const char* extension) const noexcept override;

    void labelHandle(ObjectType handleType, uint64_t handle, const char* name) noexcept override;
    void label(IHandled* handled, const char* name) noexcept override;

    Result acquireQueue(uint32_t family, QueueFlags flags, IQueue** queue) noexcept override;

    IHeap* defaultHeap() noexcept override;
    Result createHeap(BufferUsageFlags bufferUsages, TextureUsageFlags textureUsages, MemoryLocationFlags memoryLocation, IHeap** heap) noexcept override;

    Result acquireSemaphore(bool timeline, ISemaphore** semaphore) noexcept override;
    Result acquireFence(bool signaled, IFence** fence) noexcept override;

    Result createShaderModule(ShaderModuleInfo const* info, IShaderModule** shader) noexcept override;
    Result createDescriptorSetLayout(DescriptorSetLayoutInfo const* info, IDescriptorSetLayout** layout) noexcept override;
    Result createDescriptorPool(DescriptorPoolInfo const* info, IDescriptorPool** pool) noexcept override;
    Result createPipelineLayout(PipelineLayoutInfo const* info, IPipelineLayout** layout) noexcept override;
    Result createGraphicsPipeline(GraphicsPipelineInfo const* info, IPipelineCache* cache, IPipelineLayout* layout, IGraphicsPipeline** pipeline) noexcept override;
    Result createComputePipeline(ComputePipelineInfo const* info, IPipelineCache* cache, IPipelineLayout* layout, IComputePipeline** pipeline) noexcept override;

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
