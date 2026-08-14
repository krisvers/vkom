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
class VulkanManualBackbuffer final : virtual public IBackbuffer, virtual public CollectedByHeap {
private:


public:

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

    std::vector<VkImage> _backbufferImages = {};
    std::vector<IBackbuffer*> _backbuffers = {};
    std::vector<DummyWSISynchronizationPrimitives> _dummyWSISync = {};

    bool acquireDummyWSISync(DummyWSISynchronizationPrimitives& primitives);
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