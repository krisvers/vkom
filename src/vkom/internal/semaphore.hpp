#pragma once

#include <vkom/semaphore.hpp>

#include <limits>

#include <vkom/device.hpp>
#include <vkom/adapter.hpp>
#include <vkom/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

/*
*   NOTE: while this class implements ITimelineSemaphore, instances are not guaranteed to advertise the interface
*         only instances that wrap around an actual Vulkan timeline semaphore will advertise it
*/

class VulkanSemaphore final : virtual public ISemaphore, virtual public ITimelineSemaphore, virtual public CollectedByHeap {
private:
    bool _inheritedHandle = false;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanSemaphoreData _semaphoreData;

public:
    VulkanSemaphore(bool inheritedHandle, IDevice* device, VulkanSemaphoreData const& semaphoreData);
    ~VulkanSemaphore();

    /* ITimelineSemaphore */
    Result wait(uint64_t value, uint64_t timeout = std::numeric_limits<uint64_t>::max()) noexcept override;
    Result signal(uint64_t value) noexcept override;
    uint64_t counter() const noexcept override;

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
