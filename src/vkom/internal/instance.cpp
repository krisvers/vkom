#include "vkom/dynlib.hpp"
#include <vkom/internal/instance.hpp>

#include <cstring>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/adapter.hpp>

namespace vkom {

namespace internal {

VulkanInstance::VulkanInstance(bool debug, uint32_t vkApiVersion, bool inheritedHandle, IDynlib* dynlib, VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanInstanceFunctionPointers const& functionPointers, std::vector<const char*> const& enabledExtensions, PFN_vkDebugUtilsMessengerCallbackEXT vkDebugUtilsMessengerUserCallback, void* vkDebugUtilsMessengerUserData) : _debug(debug), _vkApiVersion(vkApiVersion), _inheritedHandle(inheritedHandle), _vulkanDynlib(dynlib), _vkInstance(vkInstance), _vkGetInstanceProcAddr(vkGetInstanceProcAddr), _vkAllocationCallbacks(vkAllocationCallbacks), _functionPointers(functionPointers), _enabledExtensions(enabledExtensions) {
    VulkanAdapterFunctionPointers adapterFunctionPointers = {};
    if (!adapterFunctionPointers.physical10.load(vkInstance, vkGetInstanceProcAddr)) {
        throw std::runtime_error("Failed to load physical device functions");
    }

    adapterFunctionPointers.physical11.load(vkInstance, vkGetInstanceProcAddr);
    adapterFunctionPointers.surfaceKHR.load(vkInstance, vkGetInstanceProcAddr);

    uint32_t physicalDeviceCount;
    if (_functionPointers.instance10.vkEnumeratePhysicalDevices(_vkInstance, &physicalDeviceCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("vkEnumeratePhysicalDevices failed");
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    if (_functionPointers.instance10.vkEnumeratePhysicalDevices(_vkInstance, &physicalDeviceCount, &physicalDevices[0]) != VK_SUCCESS) {
        throw std::runtime_error("vkEnumeratePhysicalDevices failed");
    }

    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {};
    debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCreateInfo.messageSeverity = (VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);
    debugUtilsMessengerCreateInfo.messageType = (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);
    debugUtilsMessengerCreateInfo.pfnUserCallback = (vkDebugUtilsMessengerUserCallback != nullptr) ? vkDebugUtilsMessengerUserCallback : debugUtilsMessengerCallback;
    debugUtilsMessengerCreateInfo.pUserData = (vkDebugUtilsMessengerUserCallback != nullptr) ? vkDebugUtilsMessengerUserData : this;

    if (debug && _functionPointers.debugUtilsEXT.vkCreateDebugUtilsMessengerEXT != nullptr) {
        _functionPointers.debugUtilsEXT.vkCreateDebugUtilsMessengerEXT(_vkInstance, &debugUtilsMessengerCreateInfo, _vkAllocationCallbacks, &_vkDebugUtilsMessenger);
    }

    for (VkPhysicalDevice phys : physicalDevices) {
        VulkanAdapter* adapter = new VulkanAdapter(debug, this, phys, adapterFunctionPointers);
        _adapters.push_back(adapter);
        _children.push_back(adapter);
    }
}

VulkanInstance::~VulkanInstance() {
    if (_vkDebugUtilsMessenger != VK_NULL_HANDLE && _functionPointers.debugUtilsEXT.vkDestroyDebugUtilsMessengerEXT != nullptr) {
        _functionPointers.debugUtilsEXT.vkDestroyDebugUtilsMessengerEXT(_vkInstance, _vkDebugUtilsMessenger, _vkAllocationCallbacks);
    }

    if (!_inheritedHandle && _functionPointers.instance10.vkDestroyInstance != nullptr) {
        _functionPointers.instance10.vkDestroyInstance(_vkInstance, _vkAllocationCallbacks);
    }

    if (_vulkanDynlib != nullptr) {
        _vulkanDynlib->destroy();
    }
}

/* IInstance */
void VulkanInstance::setLogCallback(InstanceLogCallbackPFN callback, void* userData) noexcept {
    _logCallback = callback;
    _logUserData = userData;
}

IAdapter* VulkanInstance::enumerateAdapters(uint32_t id) const noexcept {
    if (id >= _adapters.size()) {
        return nullptr;
    }

    return _adapters[id];
}

/* INullable */
bool VulkanInstance::isNull() const noexcept {
    return _vkInstance == nullptr;
}

/* IHandled */
uint64_t VulkanInstance::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_vkInstance);
}

ObjectType VulkanInstance::handleType() const noexcept {
    return ObjectType::Instance;
}

/* IDispatchable */
void* VulkanInstance::loadDispatchSymbol(const char* symbol) {
    return reinterpret_cast<void*>(_vkGetInstanceProcAddr(_vkInstance, symbol));
}

/* IInterface */
void* VulkanInstance::queryInterface(IID const& iid) noexcept {
    if (iid == INullable::iid()) {
        return static_cast<INullable*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IParent::iid()) {
        return static_cast<IParent*>(this);
    } else if (iid == IDispatchable::iid()) {
        return static_cast<IDispatchable*>(this);
    } else if (iid == IInstance::iid()) {
        return static_cast<IInstance*>(this);
    }

    return nullptr;
}

/* internal */
uint32_t VulkanInstance::vkApiVersion() const noexcept {
    return _vkApiVersion;
}

bool VulkanInstance::isExtensionEnabled(const char* name) const noexcept {
    for (const char* s : _enabledExtensions) {
        if (std::strcmp(s, name) == 0) {
            return true;
        }
    }

    return false;
}

VkBool32 VulkanInstance::debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types, VkDebugUtilsMessengerCallbackDataEXT const* callbackData, void* userData) {
    VulkanInstance* instance = reinterpret_cast<VulkanInstance*>(userData);
    if (instance->_logCallback != nullptr) {
        instance->_logCallback(instance, instance->_logUserData, castEnum<DebugMessageSeverityFlags>(severity), castEnum<DebugMessageTypeFlags>(types), callbackData->pMessage);
    }

    return false;
}

static IDynlib* loadVulkanDynlibDefaultPaths() {
    /* adapted from https://github.com/libsdl-org/SDL/blob/855cbec702f246661ff00a0bce9e0683012840c2/src/video/offscreen/SDL_offscreenvulkan.c#L29 */
    const char* defaultPaths[] = {
        #ifdef VKOM_PLATFORM_FAMILY_NT
        "vulkan-1.dll",
        #elif defined(VKOM_PLATFORM_FAMILY_APPLE)
        "vulkan.framework/vulkan",
        "libvulkan.1.dylib",
        "libvulkan.dylib",
        "MoltenVK.framework/MoltenVK",
        "libMoltenVK.dylib",
        #elif defined(VKOM_PLATFORM_OPENBSD)
        "libvulkan.so",
        #else
        "libvulkan.so.1",
        #endif
    };

    for (const char* path : defaultPaths) {
        IDynlib* dynlib = loadDynlib(path);
        if (dynlib != nullptr) {
            return dynlib;
        }
    }

    return nullptr;
}

}

Result createInstance(bool debug, InstanceLoaderInfo const* loaderInfo, IInstance **instance) {
    IDynlib* dynlib = nullptr;
    const char* loaderPath = nullptr;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = nullptr;
    VkInstance vkInstance = nullptr;
    VkAllocationCallbacks const* vkAllocationCallbacks = nullptr;
    PFN_vkDebugUtilsMessengerCallbackEXT vkDebugUtilsMessengerCallback = nullptr;
    void* vkDebugUtilsMessengerUserData = nullptr;

    if (loaderInfo != nullptr) {
        loaderPath = loaderInfo->loaderPath;
        vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(loaderInfo->vkGetInstanceProcAddr);
        vkInstance = reinterpret_cast<VkInstance>(loaderInfo->vkInstanceHandle);
        vkAllocationCallbacks = reinterpret_cast<VkAllocationCallbacks*>(loaderInfo->vkAllocationCallbacks);
        vkDebugUtilsMessengerCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(loaderInfo->vkDebugUtilsMessengerCallback);
        vkDebugUtilsMessengerUserData = loaderInfo->vkDebugUtilsMessengerUserData;
    }

    if (vkGetInstanceProcAddr == nullptr) {
        if (dynlib == nullptr) {
            if (loaderPath != nullptr) {
                /* user provided library path failed */
                dynlib = loadDynlib(loaderPath);
            }

            if (dynlib == nullptr) {
                dynlib = internal::loadVulkanDynlibDefaultPaths();
            }

            if (dynlib == nullptr) {
                /* failed to load Vulkan dynlib */
                return Result::ErrorInitializationFailed;
            }
        }

        vkGetInstanceProcAddr = dynlib->loadSymbol<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        if (vkGetInstanceProcAddr == nullptr) {
            /* loaded library is not a valid Vulkan dynlib */
            dynlib->destroy();
            return Result::ErrorInitializationFailed;
        }
    }

    uint32_t vkApiVersion = VK_API_VERSION_1_0;

    PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));
    if (vkEnumerateInstanceVersion != nullptr) {
        vkEnumerateInstanceVersion(&vkApiVersion);
    }

    PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceExtensionProperties"));
    if (vkEnumerateInstanceExtensionProperties == nullptr) {
        /* loaded library is not a valid Vulkan dynlib */
        dynlib->destroy();
        return Result::ErrorInitializationFailed;
    }

    uint32_t availableExtensionCount;
    if (vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr) != VK_SUCCESS) {
        /* vkEnumerateInstanceExtensionProperties failed */
        dynlib->destroy();
        return Result::ErrorInitializationFailed;
    }

    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    if (vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, &availableExtensions[0]) != VK_SUCCESS) {
        /* vkEnumerateInstanceExtensionProperties failed */
        dynlib->destroy();
        return Result::ErrorInitializationFailed;
    }

    bool portabilityEnumeration = false;

    std::vector<const char*> enabledExtensions = {};
    for (VkExtensionProperties const& props : availableExtensions) {
        if (std::strcmp(props.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
            enabledExtensions.push_back(props.extensionName);
        } else if (std::strcmp(props.extensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0) {
            #ifdef VKOM_PLATFORM_FAMILY_APPLE
            enabledExtensions.push_back(props.extensionName);
            portabilityEnumeration = true;
            #endif
        } else if (std::strcmp(props.extensionName, VK_KHR_SURFACE_EXTENSION_NAME) == 0) {
            enabledExtensions.push_back(props.extensionName);
        } else if (std::strcmp(props.extensionName, "VK_KHR_win32_surface") == 0) {
            enabledExtensions.push_back(props.extensionName);
        } else if (std::strcmp(props.extensionName, "VK_KHR_xlib_surface") == 0) {
            enabledExtensions.push_back(props.extensionName);
        } else if (std::strcmp(props.extensionName, "VK_KHR_xcb_surface") == 0) {
            enabledExtensions.push_back(props.extensionName);
        } else if (std::strcmp(props.extensionName, "VK_KHR_wayland_surface") == 0) {
            enabledExtensions.push_back(props.extensionName);
        } else if (std::strcmp(props.extensionName, "VK_EXT_metal_surface") == 0) {
            enabledExtensions.push_back(props.extensionName);
        } else if (std::strcmp(props.extensionName, "VK_MVK_macos_surface") == 0) {
            enabledExtensions.push_back(props.extensionName);
        } else if (std::strcmp(props.extensionName, "VK_MVK_ios_surface") == 0) {
            enabledExtensions.push_back(props.extensionName);
        }
    }

    bool inheritedVkInstance = (vkInstance != nullptr);
    if (vkInstance == nullptr) {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "vkom";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.pEngineName = "vkom";
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.apiVersion = vkApiVersion;

        const char* validationLayer = "VK_LAYER_KHRONOS_validation";

        void* next = nullptr;

        VkInstanceCreateFlags createFlags = {};
        if (portabilityEnumeration) {
            createFlags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.flags = createFlags;
        createInfo.pNext = next;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = debug ? 1 : 0;
        createInfo.ppEnabledLayerNames = &validationLayer;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
        createInfo.ppEnabledExtensionNames = enabledExtensions.empty() ? nullptr : &enabledExtensions[0];

        PFN_vkCreateInstance vkCreateInstance = dynlib->loadSymbol<PFN_vkCreateInstance>("vkCreateInstance");
        if (vkCreateInstance == nullptr) {
            /* loaded library is not a valid Vulkan dynlib */
            dynlib->destroy();
            return Result::ErrorInitializationFailed;
        }

        VkResult result = vkCreateInstance(&createInfo, vkAllocationCallbacks, &vkInstance);
        if (result != VK_SUCCESS) {
            /* failed to create Vulkan instance */
            dynlib->destroy();
            return internal::castEnum<Result>(result);
        }
    }

    internal::VulkanInstanceFunctionPointers functionPointers = {};
    if (!functionPointers.instance10.load(vkInstance, vkGetInstanceProcAddr)) {
        /* loaded library is not a valid Vulkan dynlib */
        if (!inheritedVkInstance && functionPointers.instance10.vkDestroyInstance != nullptr) {
            functionPointers.instance10.vkDestroyInstance(vkInstance, vkAllocationCallbacks);
        }

        dynlib->destroy();
        return Result::ErrorInitializationFailed;
    }

    functionPointers.instance11.load(vkInstance, vkGetInstanceProcAddr);
    functionPointers.instance12.load(vkInstance, vkGetInstanceProcAddr);
    functionPointers.debugUtilsEXT.load(vkInstance, vkGetInstanceProcAddr);

    try {
        *instance = new internal::VulkanInstance(debug, vkApiVersion, inheritedVkInstance, dynlib, vkInstance, vkGetInstanceProcAddr, vkAllocationCallbacks, functionPointers, enabledExtensions, vkDebugUtilsMessengerCallback, vkDebugUtilsMessengerUserData);
    } catch (std::runtime_error const& err) {
        dynlib->destroy();
        return Result::ErrorInitializationFailed;
    }

    return Result::Success;
}

}
