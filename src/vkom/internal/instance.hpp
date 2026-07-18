#pragma once

#include <vector>

#include <vkom/enums.hpp>
#include <vkom/instance.hpp>
#include <vkom/dynlib.hpp>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

namespace vkom {

namespace internal {

class VulkanInstance : public IInstance {
private:
    bool _debug = false;
    IDynlib* _vulkanDynlib = nullptr;
    VkInstance _vkInstance = nullptr;
    PFN_vkGetInstanceProcAddr _vkGetInstanceProcAddr = nullptr;

    /* ICollected */
    uint32_t _referenceCount = 1;

    /* IParent */
    std::vector<IChild*> _children;

public:
    VulkanInstance(bool debug, IDynlib* dynlib, VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr);
    ~VulkanInstance();

    /* IInstance */


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
