#include <vkom/internal/instance.hpp>

namespace vkom {

namespace internal {

VulkanInstance::VulkanInstance(bool debug, IDynlib* dynlib, VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr) : _debug(debug), _vulkanDynlib(dynlib), _vkInstance(vkInstance), _vkGetInstanceProcAddr(vkGetInstanceProcAddr) {

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

    if (loaderInfo != nullptr) {
        loaderPath = loaderInfo->loaderPath;
        vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(loaderInfo->vkGetInstanceProcAddr);
        vkInstance = reinterpret_cast<VkInstance>(loaderInfo->vkInstanceHandle);
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

    if (vkInstance == nullptr) {
        /* create a Vulkan instance */
    }

    *instance = new internal::VulkanInstance(debug, dynlib, vkInstance, vkGetInstanceProcAddr);
    return Result::Success;
}

}
