#include <vkom/internal/instance.hpp>

#include <cstring>

#include <vkom/internal/adapter.hpp>

namespace vkom {

namespace internal {

VulkanInstance::VulkanInstance(bool debug, uint32_t vkApiVersion, bool inheritedHandle, IDynlib* dynlib, VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanInstanceFunctionPointers const& functionPointers, std::vector<const char*> const& enabledExtensions) : _debug(debug), _vkApiVersion(vkApiVersion), _inheritedHandle(inheritedHandle), _vulkanDynlib(dynlib), _vkInstance(vkInstance), _vkGetInstanceProcAddr(vkGetInstanceProcAddr), _vkAllocationCallbacks(vkAllocationCallbacks), _functionPointers(functionPointers), _enabledExtensions(enabledExtensions) {
    VulkanAdapterFunctionPointers adapterFunctionPointers = {};
    if (!adapterFunctionPointers.physical10.load(vkInstance, vkGetInstanceProcAddr)) {
        throw std::runtime_error("Failed to load physical device functions");
    }

    uint32_t physicalDeviceCount;
    if (_functionPointers.instance10.vkEnumeratePhysicalDevices(_vkInstance, &physicalDeviceCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("vkEnumeratePhysicalDevices failed");
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    if (_functionPointers.instance10.vkEnumeratePhysicalDevices(_vkInstance, &physicalDeviceCount, &physicalDevices[0]) != VK_SUCCESS) {
        throw std::runtime_error("vkEnumeratePhysicalDevices failed");
    }

    for (VkPhysicalDevice phys : physicalDevices) {
        VulkanAdapter* adapter = new VulkanAdapter(debug, this, phys, adapterFunctionPointers);
        _adapters.push_back(adapter);
        _children.push_back(adapter);
    }
}

VulkanInstance::~VulkanInstance() {
    for (IChild* child : _children) {
        if (child->supportsInterface(ICOLLECTED_IID)) {
            ICollected* collected = reinterpret_cast<ICollected*>(child);
            if (collected->release() != 0) {
                /* TODO: report mismanaged references */
            }
        }
    }

    for (VulkanAdapter* adapter : _adapters) {
        delete adapter;
    }

    if (!_inheritedHandle && _functionPointers.instance10.vkDestroyInstance != nullptr) {
        _functionPointers.instance10.vkDestroyInstance(_vkInstance, _vkAllocationCallbacks);
    }

    if (_vulkanDynlib != nullptr) {
        _vulkanDynlib->destroy();
    }
}

/* IInstance */
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

/* ICollected */
uint32_t VulkanInstance::release() {
    if (_referenceCount == 0) {
        return 0;
    }

    _referenceCount -= 1;
    if (_referenceCount == 0) {
        delete this;
        return 0;
    }

    return _referenceCount;
}

uint32_t VulkanInstance::retain() {
    _referenceCount += 1;
    return _referenceCount;
}

/* IParent */
bool VulkanInstance::hasChild(IChild const* child) const noexcept {
    for (IChild const* c : _children) {
        if (c == child) {
            return true;
        }
    }

    return false;
}

IChild* VulkanInstance::enumerateChildren(uint32_t id) const noexcept {
    if (id >= _children.size()) {
        return nullptr;
    }

    return _children[id];
}

/* IDispatchable */
void* VulkanInstance::loadDispatchSymbol(const char* symbol) {
    return reinterpret_cast<void*>(_vkGetInstanceProcAddr(_vkInstance, symbol));
}

/* IInterface */
bool VulkanInstance::supportsInterface(IID const& iid) const noexcept {
    return IInstance::supportsInterface(iid);
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

static IDynlib* loadVulkanDynlibDefaultPaths() {
    /* TODO: attempt to load from possible default paths */
    return nullptr;
}

}

Result createInstance(bool debug, InstanceLoaderInfo const* loaderInfo, IInstance **instance) {
    IDynlib* dynlib = nullptr;
    const char* loaderPath = nullptr;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = nullptr;
    VkInstance vkInstance = nullptr;
    VkAllocationCallbacks const* vkAllocationCallbacks = nullptr;

    if (loaderInfo != nullptr) {
        loaderPath = loaderInfo->loaderPath;
        vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(loaderInfo->vkGetInstanceProcAddr);
        vkInstance = reinterpret_cast<VkInstance>(loaderInfo->vkInstanceHandle);
        vkAllocationCallbacks = reinterpret_cast<VkAllocationCallbacks*>(loaderInfo->vkAllocationCallbacks);
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

    std::vector<const char*> availableExtensions(availableExtensionCount);
    if (vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, &availableExtensions[0]) != VK_SUCCESS) {
        /* vkEnumerateInstanceExtensionProperties failed */
        dynlib->destroy();
        return Result::ErrorInitializationFailed;
    }

    std::vector<const char*> enabledExtensions = {};

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

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
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

        if (vkCreateInstance(&createInfo, vkAllocationCallbacks, &vkInstance) != VK_SUCCESS) {
            /* failed to create Vulkan instance */
            dynlib->destroy();
            return Result::ErrorInitializationFailed;
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

    *instance = new internal::VulkanInstance(debug, vkApiVersion, inheritedVkInstance, dynlib, vkInstance, vkGetInstanceProcAddr, vkAllocationCallbacks, functionPointers, enabledExtensions);
    return Result::Success;
}

}
