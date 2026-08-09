#include <vkom/internal/surface.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/instance.hpp>
#include <vkom/internal/vksurface.hpp>

namespace vkom {

namespace internal {

VulkanSurface::VulkanSurface(bool inheritedHandle, IInstance* instance, SurfaceWSIInfo const& info, VulkanSurfaceData const& surfaceData) : _inheritedHandle(inheritedHandle), _instance(instance), _info(info), _surfaceData(surfaceData) {
    _instance->retain();
}

VulkanSurface::~VulkanSurface() {
    _instance->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR = _instance->loadDispatchSymbol<PFN_vkDestroySurfaceKHR>("vkDestroySurfaceKHR");
        if (vkDestroySurfaceKHR != nullptr) {
            vkDestroySurfaceKHR(_surfaceData.instanceData.vkInstance, _surfaceData.vkSurface, _surfaceData.instanceData.vkAllocationCallbacks);
        }
    }

    _instance->release();
}

/* ISurface */
void VulkanSurface::getInfo(SurfaceWSIInfo* info) const noexcept {
    *info = _info;
}

/* IHandled */
uint64_t VulkanSurface::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_surfaceData.vkSurface);
}

ObjectType VulkanSurface::handleType() const noexcept {
    return ObjectType::SurfaceKHR;
}

void const* VulkanSurface::vkData() const noexcept {
    return reinterpret_cast<void const*>(&_surfaceData);
}

/* IChild */
IParent* VulkanSurface::parent() const noexcept {
    return _instance->queryInterface<IParent>();
}

/* IInterface */
void* VulkanSurface::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == ISurface::iid()) {
        return static_cast<ISurface*>(this);
    }

    return nullptr;
}

}

}
