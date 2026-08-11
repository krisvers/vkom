#pragma once

#include <vkom/fence.hpp>

#include <vkom/device.hpp>
#include <vkom/adapter.hpp>
#include <vkom/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

class VulkanFence final : virtual public IFence, virtual public CollectedByHeap {
private:
    bool _inheritedHandle = false;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanFenceData _fenceData;

public:
    VulkanFence(bool inheritedHandle, IDevice* device, VulkanFenceData const& fenceData);
    ~VulkanFence();

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

}

}