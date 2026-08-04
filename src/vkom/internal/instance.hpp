#pragma once

#include <vector>
#include <unordered_map>

#include <vkom/enums.hpp>
#include <vkom/instance.hpp>
#include <vkom/dynlib.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vulkan.hpp>

namespace vkom {

namespace internal {

class VulkanAdapter;
class VulkanDevice;

class VulkanInstance final : virtual public IInstance, virtual public ParentByVector, virtual public CollectedByHeap {
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

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    /* IDispatchable */
    void* loadDispatchSymbol(const char* symbol) override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

}

}
