#include "vkom/enums.hpp"
#include "vkom/internal/vma.hpp"
#include <vkom/internal/buffer.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/heap.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

VulkanBuffer::VulkanBuffer(bool inheritedHandle, bool alias, IHeap* heap, BufferInfo const& info, VulkanBufferData const& bufferData) : _inheritedHandle(inheritedHandle), _alias(alias), _heap(heap), _device(_heap->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _info(info), _bufferData(bufferData) {
    _heap->retain();
}

VulkanBuffer::~VulkanBuffer() {
    _device->waitIdle();
    ParentByVector::disownAll();
    _heap->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        vmaDestroyBuffer(_bufferData.heapData.deviceData.vmaAllocator, _bufferData.vkBuffer, _bufferData.vmaAllocation);
    }

    _heap->release();
}

/* IBuffer */
void VulkanBuffer::getInfo(BufferInfo* info) const noexcept {
    *info = _info;
}

uint64_t VulkanBuffer::deviceAddress() const noexcept {
    /* TODO: device 1.2 function pointers */
    if (_bufferData.heapData.deviceData.functionPointers.device12.vkGetBufferDeviceAddress == nullptr) {
        return 0;
    }

    VkBufferDeviceAddressInfo addressInfo = {};
    addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    addressInfo.buffer = _bufferData.vkBuffer;

    VkDeviceAddress address = _bufferData.heapData.deviceData.functionPointers.device12.vkGetBufferDeviceAddress(_bufferData.heapData.deviceData.vkDevice, &addressInfo);
    return static_cast<uint64_t>(address);
}

Result VulkanBuffer::createView(BufferViewInfo const* info, IBufferView** view) noexcept {
    /* TODO: */
    return Result::ErrorUnknown;
}

/* IResource */
bool VulkanBuffer::isAlias() const noexcept {
    return _alias;
}

void VulkanBuffer::getAllocationInfo(ResourceAllocationInfo* info) const noexcept {
    VkMemoryRequirements memoryRequirements;
    _bufferData.heapData.deviceData.functionPointers.device10.vkGetBufferMemoryRequirements(_bufferData.heapData.deviceData.vkDevice, _bufferData.vkBuffer, &memoryRequirements);

    info->alignment = memoryRequirements.alignment;
    info->resourceSize = _info.size;
    info->allocationLocalOffset = _bufferData.vmaAllocationInfo2.allocationInfo.offset;
    info->allocationLocalSize = _bufferData.vmaAllocationInfo2.allocationInfo.size;
}

void* VulkanBuffer::map() noexcept {
    void* mapped;
    if (vmaMapMemory(_bufferData.heapData.deviceData.vmaAllocator, _bufferData.vmaAllocation, &mapped) != VK_SUCCESS) {
        return nullptr;
    }

    return mapped;
}

void VulkanBuffer::unmap() noexcept {
    vmaUnmapMemory(_bufferData.heapData.deviceData.vmaAllocator, _bufferData.vmaAllocation);
}

/* IHandled */
uint64_t VulkanBuffer::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_bufferData.vkBuffer);
}

ObjectType VulkanBuffer::handleType() const noexcept {
    return ObjectType::Buffer;
}

void const* VulkanBuffer::vkData() const noexcept {
    return reinterpret_cast<void const*>(&_bufferData);
}

/* IChild */
IParent* VulkanBuffer::parent() const noexcept {
    return _heap->queryInterface<IParent>();
}

/* IInterface */
void* VulkanBuffer::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IParent::iid()) {
        return static_cast<IParent*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IResource::iid()) {
        return static_cast<IResource*>(this);
    } else if (iid == IBuffer::iid()) {
        return static_cast<IBuffer*>(this);
    } else if (iid == IIndirectBuffer::iid()) {
        return static_cast<IBuffer*>(this);
    } else if (iid == IIndexBuffer::iid()) {
        if ((_info.usage & BufferUsageFlags::IndexBuffer) != BufferUsageFlags::None) {
            return static_cast<IIndexBuffer*>(this);
        }

        return nullptr;
    } else if (iid == IVertexBuffer::iid()) {
        if ((_info.usage & BufferUsageFlags::VertexBuffer) != BufferUsageFlags::None) {
            return static_cast<IVertexBuffer*>(this);
        }

        return nullptr;
    } else if (iid == IUniformBuffer::iid()) {
        if ((_info.usage & BufferUsageFlags::UniformBuffer) != BufferUsageFlags::None || (_info.usage & BufferUsageFlags::UniformTexelBuffer) != BufferUsageFlags::None) {
            return static_cast<IUniformBuffer*>(this);
        }

        return nullptr;
    } else if (iid == IStorageBuffer::iid()) {
        if ((_info.usage & BufferUsageFlags::StorageBuffer) != BufferUsageFlags::None || (_info.usage & BufferUsageFlags::StorageTexelBuffer) != BufferUsageFlags::None) {
            return static_cast<IStorageBuffer*>(this);
        }

        return nullptr;
    } else if (iid == ITexelBuffer::iid()) {
        if ((_info.usage & BufferUsageFlags::UniformTexelBuffer) != BufferUsageFlags::None || (_info.usage & BufferUsageFlags::StorageTexelBuffer) != BufferUsageFlags::None) {
            return static_cast<ITexelBuffer*>(this);
        }

        return nullptr;
    } else if (iid == IDeviceAddressBuffer::iid()) {
        if ((_info.usage & BufferUsageFlags::ShaderDeviceAddress) != BufferUsageFlags::None) {
            return static_cast<IDeviceAddressBuffer*>(this);
        }

        return nullptr;
    }

    return nullptr;
}

}

}
