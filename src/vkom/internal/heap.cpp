#include <vkom/internal/heap.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/resource.hpp>
#include <vkom/internal/buffer.hpp>
#include <vkom/internal/texture.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

namespace vkom {

namespace internal {

VulkanHeap::VulkanHeap(bool debug, bool inheritedHandle, VulkanDevice* device, VmaPool vmaPool, VkAllocationCallbacks const* vkAllocationCallbacks) : _debug(debug), _inheritedHandle(inheritedHandle), _device(device), _adapter(static_cast<VulkanAdapter*>(_device->parent())), _instance(static_cast<VulkanInstance*>(_adapter->parent())), _vmaPool(vmaPool), _vkAllocationCallbacks(vkAllocationCallbacks) {}

VulkanHeap::~VulkanHeap() {
    _device->waitIdle();

    for (uint32_t id = 0; true; id += 1) {
        IChild* child = _device->enumerateChildren(id);
        if (child == nullptr) {
            break;
        }

        /* TODO: */
    }

    for (IChild* child : _children) {
        ICollected* collected = child->queryInterface<ICollected>();
        if (collected != nullptr) {
            if (collected->release() != 0) {
                /* TODO: report mismanaged references */
            }
        }
    }

    if (!_inheritedHandle && _vmaPool != nullptr) {
        vmaDestroyPool(_device->_vmaAllocator, _vmaPool);
    }
}

/* IHeap */
Result VulkanHeap::createBuffer(BufferInfo const* info, IBuffer** buffer) noexcept {
    return Result::ErrorUnknown;
}

Result VulkanHeap::createAliasedBuffer(BufferInfo const* info, ResourceAliasingInfo const* aliasingInfo, IBuffer** buffer) noexcept {
    return Result::ErrorUnknown;
}

Result VulkanHeap::createTexture(TextureInfo const* info, ITexture** texture) noexcept {
    return Result::ErrorUnknown;
}

Result VulkanHeap::createAliasedTexture(TextureInfo const* info, ResourceAliasingInfo const* aliasingInfo, ITexture** texture) noexcept {
    return Result::ErrorUnknown;
}

/* INullable */
bool VulkanHeap::isNull() const noexcept {
    return (_vmaPool != nullptr);
}

/* IHandled */
uint64_t VulkanHeap::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_vmaPool);
}

ObjectType VulkanHeap::handleType() const noexcept {
    return ObjectType::Unknown;
}

/* ICollected */
uint32_t VulkanHeap::release() {
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

uint32_t VulkanHeap::retain() {
    _referenceCount += 1;
    return _referenceCount;
}

/* IParent */
bool VulkanHeap::hasChild(IChild const* child) const noexcept {
    for (IChild const* c : _children) {
        if (c == child) {
            return true;
        }
    }

    return false;
}

IChild* VulkanHeap::enumerateChildren(uint32_t id) const noexcept {
    if (id >= _children.size()) {
        return nullptr;
    }

    return _children[id];
}

/* IChild */
IParent* VulkanHeap::parent() const noexcept {
    return _device;
}

/* IInterface */
void* VulkanHeap::queryInterface(IID const& iid) noexcept {
    if (iid == INullable::iid()) {
        return static_cast<INullable*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IParent::iid()) {
        return static_cast<IParent*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IHeap::iid()) {
        return static_cast<IHeap*>(this);
    }

    return nullptr;
}

}

}
