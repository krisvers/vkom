#include "vkom/instance.hpp"
#include "vkom/internal/object.hpp"
#include "vkom/internal/vkdata.hpp"
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

VulkanHeap::VulkanHeap(bool inheritedHandle, IDevice* device, VulkanHeapData const& heapData) : _inheritedHandle(inheritedHandle), _device(device), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _heapData(heapData) {}

VulkanHeap::~VulkanHeap() {
    _device->waitIdle();
    ParentByVector::disownAll();

    if (!_inheritedHandle && _heapData.vmaPool != nullptr) {
        vmaDestroyPool(_heapData.deviceData.vmaAllocator, _heapData.vmaPool);
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

/* IHandled */
uint64_t VulkanHeap::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_heapData.vmaPool);
}

ObjectType VulkanHeap::handleType() const noexcept {
    return ObjectType::Unknown;
}

/* IChild */
IParent* VulkanHeap::parent() const noexcept {
    return _device;
}

/* IInterface */
void* VulkanHeap::queryInterface(IID const& iid) noexcept {
    if (iid == IHandled::iid()) {
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
