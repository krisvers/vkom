#pragma once

#include <vector>

#include <vkom/enums.hpp>
#include <vkom/adapter.hpp>

#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vulkan.hpp>

namespace vkom {

namespace internal {

struct VulkanAdapterFunctionPointers {
    PhysicalDeviceFunctionPointers10 physical10;
    PhysicalDeviceFunctionPointers11 physical11;
};

class VulkanInstance;
class VulkanDevice;

class VulkanAdapter final : public IAdapter {
private:
    bool _debug = false;
    VulkanInstance* _instance = nullptr;
    VkPhysicalDevice _vkPhysicalDevice = nullptr;
    PFN_vkGetInstanceProcAddr _vkGetInstanceProcAddr = nullptr;
    VkAllocationCallbacks const* _vkAllocationCallbacks = nullptr;
    VulkanAdapterFunctionPointers _functionPointers = {};
    std::vector<VkExtensionProperties> _availableExtensions = {};

    /* IAdapter */
    AdapterInfo _info = {};
    AdapterFeatures _features = {};
    AdapterLimits _limits = {};

    /* IParent */
    std::vector<IChild*> _children = {};

    friend class VulkanDevice;

    bool isExtensionAvailable(const char* name) const noexcept;

public:
    VulkanAdapter(bool debug, VulkanInstance* instance, VkPhysicalDevice vkPhysicalDevice, VulkanAdapterFunctionPointers const& functionPointers);
    ~VulkanAdapter();

    /* IAdapter */
    void queryInfo(AdapterInfo* info) const noexcept override;
    void queryFeatures(AdapterFeatures* features) const noexcept override;
    void queryLimits(AdapterLimits* limits) const noexcept override;

    Result createDevice(IDevice** device) override;

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    /* IParent */
    bool hasChild(IChild const* child) const noexcept override;
    IChild* enumerateChildren(uint32_t id) const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IDispatchable */
    void* loadDispatchSymbol(const char* symbol) override;

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

}

}
