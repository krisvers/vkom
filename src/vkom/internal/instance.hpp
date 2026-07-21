#pragma once

#include <vector>
#include <unordered_map>

#include <vkom/enums.hpp>
#include <vkom/instance.hpp>
#include <vkom/dynlib.hpp>

#include <vkom/internal/funcptrs.hpp>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

namespace vkom {

namespace internal {

struct VulkanInstanceFunctionPointers {
    InstanceFunctionPointers10 instance10;
    InstanceFunctionPointers11 instance11;
    InstanceFunctionPointers12 instance12;
};

class VulkanAdapter;

class VulkanInstance : public IInstance {
private:
    bool _debug = false;
    bool _inheritedHandle = false;
    IDynlib* _vulkanDynlib = nullptr;
    uint32_t _vkApiVersion = 0;
    VkInstance _vkInstance = nullptr;
    PFN_vkGetInstanceProcAddr _vkGetInstanceProcAddr = nullptr;
    VkAllocationCallbacks const* _vkAllocationCallbacks = nullptr;
    VulkanInstanceFunctionPointers _functionPointers = {};
    std::vector<const char*> _enabledExtensions = {};

    /* IInstance */
    std::vector<VulkanAdapter*> _adapters = {};

    /* ICollected */
    uint32_t _referenceCount = 1;

    /* IParent */
    std::vector<IChild*> _children = {};

    friend class VulkanAdapter;

    uint32_t vkApiVersion() const noexcept;
    bool isExtensionEnabled(const char* name) const noexcept;

public:
    VulkanInstance(bool debug, uint32_t vkApiVersion, bool inheritedHandle, IDynlib* dynlib, VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanInstanceFunctionPointers const& functionPointers, std::vector<const char*> const& enabledExtensions);
    ~VulkanInstance();

    /* IInstance */
    IAdapter* enumerateAdapters(uint32_t id) const noexcept override;

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

    /* IDispatchable */
    void* loadDispatchSymbol(const char* symbol) override;

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

}

}
