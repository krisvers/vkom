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

VulkanBufferView::VulkanBufferView(bool inheritedHandle, IBuffer* buffer, BufferViewInfo const& info, VulkanBufferViewData const& viewData) : _inheritedHandle(inheritedHandle), _buffer(buffer), _heap(_buffer->parent<IHeap>()), _device(_heap->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _info(info), _viewData(viewData) {
    _buffer->retain();

    _device->label(this, fmt::label(_device, this, "{} offset, {} range ({})", info.offset, info.range, info.format).c_str());
}

VulkanBufferView::~VulkanBufferView() {
    _buffer->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle && _viewData.vkBufferView != VK_NULL_HANDLE) {
        _viewData.bufferData.heapData.deviceData.functionPointers.device10.vkDestroyBufferView(_viewData.bufferData.heapData.deviceData.vkDevice, _viewData.vkBufferView, _viewData.bufferData.heapData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    _buffer->release();
}

/* IBufferView */
void VulkanBufferView::getInfo(BufferViewInfo* info) const noexcept {
    *info = _info;
}

/* IHandled */
uint64_t VulkanBufferView::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_viewData.vkBufferView);
}

ObjectType VulkanBufferView::handleType() const noexcept {
    return ObjectType::ImageView;
}

void const* VulkanBufferView::vkData() const noexcept {
    return &_viewData;
}

/* IChild */
IParent* VulkanBufferView::parent() const noexcept {
    return _buffer->queryInterface<IParent>();
}

/* IInterface */
void* VulkanBufferView::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IResourceView::iid()) {
        return static_cast<IResourceView*>(this);
    } else if (iid == IBufferView::iid()) {
        return static_cast<IBufferView*>(this);
    }

    return nullptr;
}

VulkanBuffer::VulkanBuffer(bool inheritedHandle, bool alias, IHeap* heap, BufferInfo const& info, VulkanBufferData const& bufferData) : _inheritedHandle(inheritedHandle), _alias(alias), _heap(heap), _device(_heap->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _info(info), _bufferData(bufferData) {
    _heap->retain();

    _device->label(this, fmt::label(_device, this, "{} bytes, {}, {} ({})", info.size, info.location, info.queueConcurrency ? "concurrent" : "exclusive", info.usage).c_str());
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
    if (info->format == Format::Undefined) {
        VulkanBufferViewData viewData = VulkanBufferViewData(_bufferData, VK_NULL_HANDLE);

        try {
            *view = new VulkanBufferView(false, this, *info, viewData);
        } catch (std::runtime_error err) {
            /* TODO: error */
            return Result::ErrorUnknown;
        }

        adopt(*view);
        return Result::Success;
    }

    VkBufferViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
    createInfo.buffer = _bufferData.vkBuffer;
    createInfo.format = castEnum<VkFormat>(info->format);
    createInfo.offset = static_cast<VkDeviceSize>(info->offset);
    createInfo.range = static_cast<VkDeviceSize>(info->range);

    VkBufferView vkBufferView;
    Result result = castEnum<Result>(_bufferData.heapData.deviceData.functionPointers.device10.vkCreateBufferView(_bufferData.heapData.deviceData.vkDevice, &createInfo, _bufferData.heapData.deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkBufferView));
    if (result != Result::Success) {
        return result;
    }

    VulkanBufferViewData viewData = VulkanBufferViewData(_bufferData, vkBufferView);

    try {
        *view = new VulkanBufferView(false, this, *info, viewData);
    } catch (std::runtime_error err) {
        /* TODO: error */
        _bufferData.heapData.deviceData.functionPointers.device10.vkDestroyBufferView(_bufferData.heapData.deviceData.vkDevice, vkBufferView, _bufferData.heapData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
        return Result::ErrorUnknown;
    }

    adopt(*view);
    return Result::Success;
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
    } else if (iid == ITransferSourceBuffer::iid()) {
        if ((_info.usage & BufferUsageFlags::TransferSource) != BufferUsageFlags::None) {
            return static_cast<ITransferSourceBuffer*>(this);
        }

        return nullptr;
    } else if (iid == ITransferDestinationBuffer::iid()) {
        if ((_info.usage & BufferUsageFlags::TransferDestination) != BufferUsageFlags::None) {
            return static_cast<ITransferDestinationBuffer*>(this);
        }

        return nullptr;
    } else if (iid == IIndirectBuffer::iid()) {
        if ((_info.usage & BufferUsageFlags::IndirectBuffer) != BufferUsageFlags::None) {
            return static_cast<IIndirectBuffer*>(this);
        }

        return nullptr;
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
    } else if (iid == IUniformTexelBuffer::iid()) {
        if ((_info.usage & BufferUsageFlags::UniformTexelBuffer) != BufferUsageFlags::None) {
            return static_cast<IUniformTexelBuffer*>(this);
        }

        return nullptr;
    } else if (iid == IStorageTexelBuffer::iid()) {
        if ((_info.usage & BufferUsageFlags::StorageTexelBuffer) != BufferUsageFlags::None) {
            return static_cast<IStorageTexelBuffer*>(this);
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
