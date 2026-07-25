#pragma once

#include <vector>
#include <unordered_map>

#include <vkom/enums.hpp>
#include <vkom/instance.hpp>
#include <vkom/dynlib.hpp>

#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vulkan.hpp>

namespace vkom {

namespace internal {

struct VulkanInstanceFunctionPointers {
    InstanceFunctionPointers10 instance10;
    InstanceFunctionPointers11 instance11;
    InstanceFunctionPointers12 instance12;
    InstanceFunctionPointersDebugUtilsEXT debugUtilsEXT;
};

class VulkanAdapter;

class VulkanInstance final : public IInstance {
private:
    bool _debug = false;
    uint32_t _vkApiVersion = 0;
    bool _inheritedHandle = false;
    IDynlib* _vulkanDynlib = nullptr;
    VkInstance _vkInstance = nullptr;
    PFN_vkGetInstanceProcAddr _vkGetInstanceProcAddr = nullptr;
    VkAllocationCallbacks const* _vkAllocationCallbacks = nullptr;
    VulkanInstanceFunctionPointers _functionPointers = {};
    std::vector<const char*> _enabledExtensions = {};
    VkDebugUtilsMessengerEXT _vkDebugUtilsMessenger = VK_NULL_HANDLE;

    /* IInstance */
    std::vector<VulkanAdapter*> _adapters = {};
    InstanceLogCallbackPFN _logCallback = nullptr;
    void* _logUserData = nullptr;

    /* ICollected */
    uint32_t _referenceCount = 1;

    /* IParent */
    std::vector<IChild*> _children = {};

    friend class VulkanAdapter;

    /* internal */
    uint32_t vkApiVersion() const noexcept;
    bool isExtensionEnabled(const char* name) const noexcept;

    static VkBool32 debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types, VkDebugUtilsMessengerCallbackDataEXT const* callbackData, void* userData);

public:
    VulkanInstance(bool debug, uint32_t vkApiVersion, bool inheritedHandle, IDynlib* dynlib, VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanInstanceFunctionPointers const& functionPointers, std::vector<const char*> const& enabledExtensions, PFN_vkDebugUtilsMessengerCallbackEXT vkDebugUtilsMessengerUserCallback, void* vkDebugUtilsMessengerUserData);
    ~VulkanInstance();

    /* IInstance */
    void setLogCallback(InstanceLogCallbackPFN callback, void* userData) noexcept override;
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
