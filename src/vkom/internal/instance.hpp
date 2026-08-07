#pragma once

#include <vector>
#include <unordered_map>

#include <vkom/enums.hpp>
#include <vkom/instance.hpp>
#include <vkom/dynlib.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

class VulkanAdapter;
class VulkanDevice;

class VulkanInstance final : virtual public IInstance, virtual public ParentByVector, virtual public CollectedByHeap {
private:
    uint32_t _vkApiVersion = 0;
    bool _inheritedHandle = false;
    IDynlib* _vulkanDynlib = nullptr;
    VulkanInstanceData _instanceData;
    std::vector<const char*> _enabledExtensions = {};
    VkDebugUtilsMessengerEXT _vkDebugUtilsMessenger = VK_NULL_HANDLE;

    /* IInstance */
    InstanceLogCallbackPFN _logCallback = nullptr;
    void* _logUserData = nullptr;

    /* internal */
    static VkBool32 debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types, VkDebugUtilsMessengerCallbackDataEXT const* callbackData, void* userData);

public:
    VulkanInstance(uint32_t vkApiVersion, bool inheritedHandle, IDynlib* dynlib, VulkanInstanceData const& instanceData, std::vector<const char*> const& enabledExtensions, PFN_vkDebugUtilsMessengerCallbackEXT vkDebugUtilsMessengerUserCallback, void* vkDebugUtilsMessengerUserData);
    ~VulkanInstance();

    /* IInstance */
    void setLogCallback(InstanceLogCallbackPFN callback, void* userData) noexcept override;
    void log(DebugMessageSeverityFlags severity, DebugMessageTypeFlags types, const char* message) noexcept override;

    bool queryExtension(const char* extension) const noexcept override;
    uint32_t queryApiVersion() const noexcept override;

    IAdapter* enumerateAdapters(uint32_t id) const noexcept override;

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    void const* vkData() const noexcept override;

    /* IDispatchable */
    void* loadDispatchSymbol(const char* symbol) override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

}

}
