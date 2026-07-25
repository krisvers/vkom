#pragma once

#include <vector>

#include <vkom/enums.hpp>
#include <vkom/device.hpp>

#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vulkan.hpp>

namespace vkom {

namespace internal {

struct VulkanDeviceFunctionPointers {
    DeviceFunctionPointers10 device10;
    DeviceFunctionPointers11 device11;
    DeviceFunctionPointers12 device12;
};

class VulkanInstance;
class VulkanAdapter;

class VulkanDevice : public IDevice {
private:
    bool _debug = false;
    bool _inheritedHandle = false;
    VulkanInstance* _instance = nullptr;
    VulkanAdapter* _adapter = nullptr;
    VkDevice _vkDevice = nullptr;
    PFN_vkGetDeviceProcAddr _vkGetDeviceProcAddr = nullptr;
    VkAllocationCallbacks const* _vkAllocationCallbacks = nullptr;
    VulkanDeviceFunctionPointers _functionPointers = {};
    std::vector<const char*> _enabledExtensions = {};

    /* ICollected */
    uint32_t _referenceCount = 1;

    /* IParent */
    std::vector<IChild*> _children = {};

public:
    VulkanDevice(bool debug, bool inheritedHandle, VulkanAdapter* adapter, VkDevice vkDevice, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanDeviceFunctionPointers const& functionPointers, std::vector<const char*> const& enabledExtensions);
    ~VulkanDevice();

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
    bool supportsInterface(IID const& iid) const noexcept override;
};

}

}