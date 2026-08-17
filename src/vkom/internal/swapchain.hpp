#pragma once

#include <vkom/swapchain.hpp>

#include <vkom/texture.hpp>
#include <vkom/fence.hpp>
#include <vkom/semaphore.hpp>
#include <vkom/device.hpp>
#include <vkom/adapter.hpp>
#include <vkom/surface.hpp>
#include <vkom/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/vkdata.hpp>

#include <vkom/internal/texture.hpp>

namespace vkom {

namespace internal {

struct DummyWSISynchronizationPrimitives {
    VkSemaphore vkBinarySemaphore = VK_NULL_HANDLE;
    VkFence vkTriggeredFence = VK_NULL_HANDLE;
};

/* NOTE: VulkanSwapchainMaintenance1PresentFence may not advertise support for IPresentIDFence due to presentId
*   not being available on certain devices and should return the maximum when the ID is requested */

class VulkanSwapchainMaintenance1PresentFence final : virtual public IPresentIDFence, virtual public CollectedByHeap {
private:
    bool _inheritedHandle = false;
    ISwapchain* _swapchain = nullptr;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    uint64_t _presentID = std::numeric_limits<uint64_t>::max();
    VulkanFenceData _fenceData;
    VulkanSwapchainData const& _swapchainData;

public:
    VulkanSwapchainMaintenance1PresentFence(bool inheritedHandle, ISwapchain* swapchain, uint64_t presentID, VulkanFenceData const& fenceData, VulkanSwapchainData const& swapchainData);
    ~VulkanSwapchainMaintenance1PresentFence();

    /* IPresentIDFence */
    uint64_t presentID() const noexcept override;

    /* IFence */
    Result wait(uint64_t timeout = std::numeric_limits<uint64_t>::max()) noexcept override;
    Result reset() noexcept override;
    bool status() const noexcept override;

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    void const* vkData() const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

/* NOTE: this is a wrapper around vkWaitForPresentKHR which in no way involves an actual VkFence
*   querying the status of this implementation will perform a wait with a really small timeout
*/
class VulkanPresentWaitPresentIDFence final : virtual public IPresentIDFence, virtual public CollectedByHeap {
private:
    ISwapchain* _swapchain = nullptr;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    uint64_t _presentID = 0;
    VulkanSwapchainData const& _swapchainData;

    static const uint64_t _defaultStatusTimeout = 1;

public:
    VulkanPresentWaitPresentIDFence(ISwapchain* swapchain, uint64_t presentID, VulkanSwapchainData const& swapchainData);
    ~VulkanPresentWaitPresentIDFence();

    /* IPresentIDFence */
    uint64_t presentID() const noexcept override;

    /* IFence */
    Result wait(uint64_t timeout = std::numeric_limits<uint64_t>::max()) noexcept override;
    Result reset() noexcept override;
    bool status() const noexcept override;

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    void const* vkData() const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

/* TODO: backbuffer */
class VulkanManualBackbuffer final : virtual public IBackbuffer, virtual public ParentByVector {
private:
    bool _inheritedHandle = false;
    bool _alias = false;
    ISwapchain* _swapchain = nullptr;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    TextureInfo _info = {};
    uint32_t _index = 0;
    VulkanTextureData _textureData;
    VulkanSwapchainData _swapchainData;

    uint32_t _referenceCount = 0;
    bool _acquired = false;
    bool _presented = false;

    /* internal */
    void releaseSwapchainImage() noexcept;

public:
    VulkanManualBackbuffer(bool inheritedHandle, ISwapchain* swapchain, TextureInfo const& info, uint32_t index, VulkanTextureData const& textureData, VulkanSwapchainData const& swapchainData);
    ~VulkanManualBackbuffer();

    /* internal */
    void recreate(TextureInfo const& info, uint32_t index, VkImage vkImage) noexcept;
    void acquire() noexcept;
    void present() noexcept;

    /* IBackbuffer */
    uint32_t index() const noexcept override;

    /* ITexture */
    void getInfo(TextureInfo* info) const noexcept override;

    Result createView(TextureViewInfo const* info, ITextureView** view) noexcept override;

    /* IResource */
    bool isAlias() const noexcept override;
    void getAllocationInfo(ResourceAllocationInfo* info) const noexcept override;

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    void const* vkData() const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* ICollected */
    uint32_t release() override;
    uint32_t retain() override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

class VulkanManualSwapchain final : virtual public ISwapchain, virtual public CollectedByHeap, virtual public ParentByVector {
private:
    bool _inheritedHandle = false;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    ISurface* _surface = nullptr;
    SwapchainInfo _info;
    VulkanSwapchainData _swapchainData;
    VulkanHeapData _backbufferHeapData;

    bool _lastRecreateMinimized = false;
    VkQueue _lastPresentQueue = VK_NULL_HANDLE;

    std::vector<VkImage> _backbufferImages = {};
    std::vector<DummyWSISynchronizationPrimitives> _dummyWSISync = {};

    Result dummyTimelineSubmit(std::vector<VkSemaphore> const& vkWaitSemaphores, std::vector<uint64_t> const& vkWaitSemaphoreValues, std::vector<VkPipelineStageFlags> const& vkWaitDstStageMasks, std::vector<VkSemaphore> const& vkSignalSemaphores, std::vector<uint64_t> const& vkSignalSemaphoreValues, VkFence vkSignalFence) noexcept;

    VulkanSwapchainMaintenance1PresentFence* acquireSwapchainMaintenance1PresentFence(uint64_t presentID) noexcept;
    VulkanPresentWaitPresentIDFence* acquirePresentWaitPresentIDFence(uint64_t presentID) noexcept;

    bool acquireDummyWSISync(DummyWSISynchronizationPrimitives& primitives) noexcept;
    void appendDummyWSISync(DummyWSISynchronizationPrimitives& primitives) noexcept;
    void releaseDummyWSISync(std::vector<DummyWSISynchronizationPrimitives>::iterator it) noexcept;
    void releaseDummyWSISync(DummyWSISynchronizationPrimitives const& primitives, bool wait = true) noexcept;

public:
    VulkanManualSwapchain(bool inheritedHandle, IDevice* device, ISurface* surface, SwapchainInfo const& info, VulkanSwapchainData const& swapchainData);
    ~VulkanManualSwapchain();

    /* ISwapchain */
    void getInfo(SwapchainInfo* info) const noexcept override;

    ISurface* surface() const noexcept override;
    IBackbuffer* enumerateBackbuffers(uint32_t id) const noexcept override;

    Result recreate(SwapchainInfo const* info) noexcept;

    Result acquireNextIndex(SemaphorePoint const* signalSemaphore, IFence* signalFence, uint32_t* index, uint64_t timeout = std::numeric_limits<uint64_t>::max()) noexcept;
    Result present(IQueue* queue, IBackbuffer* backbuffer, PresentInfo const* info, IPresentFence** fence) noexcept override;

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    void const* vkData() const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

}

}