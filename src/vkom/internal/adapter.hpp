#pragma once

#include <vector>

#include <vkom/enums.hpp>
#include <vkom/adapter.hpp>

#include <vkom/device.hpp>
#include <vkom/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

/* NOTE: IAdapter does not publicly advertise IDestructible support but DestructibleByHeap is used
 * here so that implementations of IInstance can easily disownAll using ParentByVector
 */

class VulkanAdapter final : virtual public IAdapter, virtual public ParentByVector, virtual public DestructibleByHeap {
private:
    bool _inheritedHandle = false;
    IInstance* _instance = nullptr;
    VulkanAdapterData _adapterData;
    std::vector<VkExtensionProperties> _availableExtensions = {};

    /* IAdapter */
    AdapterInfo _info = {};
    AdapterFeatures _features = {};
    AdapterLimits _limits = {};

public:
    VulkanAdapter(bool inheritedHandle, IInstance* instance, VulkanAdapterData const& adapterData);
    ~VulkanAdapter();

    /* IAdapter */
    void queryInfo(AdapterInfo* info) const noexcept override;
    void queryFeatures(AdapterFeatures* features) const noexcept override;
    void queryLimits(AdapterLimits* limits) const noexcept override;
    bool queryExtension(const char* extension) const noexcept override;
    QueueFlags queryQueueFamilyFlags(uint32_t family) const noexcept override;

    Result createDevice(IDevice** device) override;

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
