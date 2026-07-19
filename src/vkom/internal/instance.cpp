#include <vkom/internal/instance.hpp>

namespace vkom {

namespace internal {

VulkanInstance::VulkanInstance(bool debug, bool inheritedHandle, IDynlib* dynlib, VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanInstanceFunctionPointers const& functionPointers) : _debug(debug), _inheritedHandle(inheritedHandle), _vulkanDynlib(dynlib), _vkInstance(vkInstance), _vkGetInstanceProcAddr(vkGetInstanceProcAddr), _vkAllocationCallbacks(vkAllocationCallbacks), _functionPointers(functionPointers) {

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

    if (!_inheritedHandle && _functionPointers.instance10.vkDestroyInstance != nullptr) {
        _functionPointers.instance10.vkDestroyInstance(_vkInstance, _vkAllocationCallbacks);
    }

    if (_vulkanDynlib != nullptr) {
        _vulkanDynlib->destroy();
    }
}

bool VulkanInstance::isNull() const noexcept {
    return _vkInstance == nullptr;
}

uint64_t VulkanInstance::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_vkInstance);
}

ObjectType VulkanInstance::handleType() const noexcept {
    return ObjectType::Instance;
}

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

void* VulkanInstance::loadDispatchSymbol(const char* symbol) {
    return reinterpret_cast<void*>(_vkGetInstanceProcAddr(_vkInstance, symbol));
}

bool VulkanInstance::supportsInterface(IID const& iid) const noexcept {
    return IInstance::supportsInterface(iid);
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

    uint32_t maxInstanceVersion = VK_API_VERSION_1_0;

    PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion = dynlib->loadSymbol<PFN_vkEnumerateInstanceVersion>("vkEnumerateInstanceVersion");
    if (vkEnumerateInstanceVersion != nullptr) {
        vkEnumerateInstanceVersion(&maxInstanceVersion);
    }

    bool inheritedVkInstance = (vkInstance != nullptr);
    if (vkInstance == nullptr) {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "vkom";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.pEngineName = "vkom";
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.apiVersion = maxInstanceVersion;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

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

    *instance = new internal::VulkanInstance(debug, inheritedVkInstance, dynlib, vkInstance, vkGetInstanceProcAddr, vkAllocationCallbacks, functionPointers);
    return Result::Success;
}

}
