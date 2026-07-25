#include <vkom/internal/device.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

namespace vkom {

namespace internal {

VulkanDevice::VulkanDevice(bool debug, bool inheritedHandle, VulkanAdapter* adapter, VkDevice vkDevice, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanDeviceFunctionPointers const& functionPointers, std::vector<const char*> const& enabledExtensions) : _debug(debug), _inheritedHandle(inheritedHandle), _adapter(_adapter), _instance(reinterpret_cast<VulkanInstance*>(adapter->parent())), _vkDevice(vkDevice), _vkGetDeviceProcAddr(vkGetDeviceProcAddr), _vkAllocationCallbacks(vkAllocationCallbacks), _functionPointers(functionPointers), _enabledExtensions(enabledExtensions) {
    
}

VulkanDevice::~VulkanDevice() {
    for (IChild* child : _children) {
        if (child->supportsInterface(ICOLLECTED_IID)) {
            ICollected* collected = reinterpret_cast<ICollected*>(child);
            if (collected->release() != 0) {
                /* TODO: report mismanaged references */
            }
        }
    }

    if (!_inheritedHandle && _functionPointers.device10.vkDestroyDevice != nullptr) {
        _functionPointers.device10.vkDestroyDevice(_vkDevice, _vkAllocationCallbacks);
    }
}

/* INullable */
bool VulkanDevice::isNull() const noexcept {
    return (_vkDevice == nullptr);
}

/* IHandled */
uint64_t VulkanDevice::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_vkDevice);
}

ObjectType VulkanDevice::handleType() const noexcept {
    return ObjectType::Device;
}

/* ICollected */
uint32_t VulkanDevice::release() {
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

uint32_t VulkanDevice::retain() {
    _referenceCount += 1;
    return _referenceCount;
}

/* IParent */
bool VulkanDevice::hasChild(IChild const* child) const noexcept {
    for (IChild const* c : _children) {
        if (c == child) {
            return true;
        }
    }

    return false;
}

IChild* VulkanDevice::enumerateChildren(uint32_t id) const noexcept {
    if (id >= _children.size()) {
        return nullptr;
    }

    return _children[id];
}

/* IChild */
IParent* VulkanDevice::parent() const noexcept {
    return _adapter;
}

/* IDispatchable */
void* VulkanDevice::loadDispatchSymbol(const char* symbol) {
    return _instance->loadDispatchSymbol(symbol);
}

/* IInterface */
bool VulkanDevice::supportsInterface(IID const& iid) const noexcept {
    return IDevice::supportsInterface(iid);
}

}

}