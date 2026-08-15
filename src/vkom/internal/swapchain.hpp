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

    void releaseSwapchainImage() noexcept;
    Result acquireSwapchainMaintenancePresentFence(IPresentFence** fence) noexcept;
    Result acquirePresentIDFence(uint64_t presentID, IPresentIDFence** fence) noexcept;

public:
    VulkanManualBackbuffer(bool inheritedHandle, ISwapchain* swapchain, TextureInfo const& info, uint32_t index, VulkanTextureData const& textureData, VulkanSwapchainData const& swapchainData);
    ~VulkanManualBackbuffer();

    bool markAcquired() noexcept;

    /* IBackbuffer */
    uint32_t index() const noexcept override;
    Result present(PresentInfo const* info, IPresentFence** signal) noexcept override;

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

    std::vector<VkImage> _backbufferImages = {};
    std::vector<IBackbuffer*> _backbuffers = {};
    std::vector<DummyWSISynchronizationPrimitives> _dummyWSISync = {};

    bool acquireDummyWSISync(DummyWSISynchronizationPrimitives& primitives);
    void releaseDummyWSISync(std::vector<DummyWSISynchronizationPrimitives>::iterator it);
    void releaseDummyWSISync(DummyWSISynchronizationPrimitives const& primitives);

public:
    VulkanManualSwapchain(bool inheritedHandle, IDevice* device, ISurface* surface, SwapchainInfo const& info, VulkanSwapchainData const& swapchainData);
    ~VulkanManualSwapchain();

    /* ISwapchain */
    void getInfo(SwapchainInfo* info) const noexcept override;

    ISurface* surface() const noexcept override;
    IBackbuffer* enumerateBackbuffers(uint32_t id) const noexcept override;

    Result recreate(SwapchainInfo const* info) noexcept;

    Result acquireNextIndex(SemaphorePoint const* signalSemaphore, IFence* signalFence, uint32_t* index, uint64_t timeout = std::numeric_limits<uint64_t>::max()) noexcept;

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