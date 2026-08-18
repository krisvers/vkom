#include <vkom/internal/texture.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/heap.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vma.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

VulkanTexture::VulkanTexture(bool inheritedHandle, bool alias, IHeap* heap, TextureInfo const& info, VulkanTextureData const& TextureData) : _inheritedHandle(inheritedHandle), _alias(alias), _heap(heap), _device(_heap->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _info(info), _textureData(TextureData) {
    _heap->retain();
}

VulkanTexture::~VulkanTexture() {
    _device->waitIdle();
    ParentByVector::disownAll();
    _heap->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        vmaDestroyImage(_textureData.heapData.deviceData.vmaAllocator, _textureData.vkImage, _textureData.vmaAllocation);
    }

    _heap->release();
}

/* ITexture */
void VulkanTexture::getInfo(TextureInfo* info) const noexcept {
    *info = _info;
}

Result VulkanTexture::createView(TextureViewInfo const* info, ITextureView** view) noexcept {
    /* TODO: */
    return Result::ErrorUnknown;
}

/* IResource */
bool VulkanTexture::isAlias() const noexcept {
    return _alias;
}

void VulkanTexture::getAllocationInfo(ResourceAllocationInfo* info) const noexcept {
    VkMemoryRequirements memoryRequirements;
    _textureData.heapData.deviceData.functionPointers.device10.vkGetImageMemoryRequirements(_textureData.heapData.deviceData.vkDevice, _textureData.vkImage, &memoryRequirements);

    info->alignment = memoryRequirements.alignment;
    info->resourceSize = memoryRequirements.size;
    info->allocationLocalOffset = _textureData.vmaAllocationInfo2.allocationInfo.offset;
    info->allocationLocalSize = _textureData.vmaAllocationInfo2.allocationInfo.size;
}

void* VulkanTexture::map() noexcept {
    void* mapped;
    if (vmaMapMemory(_textureData.heapData.deviceData.vmaAllocator, _textureData.vmaAllocation, &mapped) != VK_SUCCESS) {
        return nullptr;
    }

    return mapped;
}

void VulkanTexture::unmap() noexcept {
    vmaUnmapMemory(_textureData.heapData.deviceData.vmaAllocator, _textureData.vmaAllocation);
}

/* IHandled */
uint64_t VulkanTexture::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_textureData.vkImage);
}

ObjectType VulkanTexture::handleType() const noexcept {
    return ObjectType::Image;
}

void const* VulkanTexture::vkData() const noexcept {
    return reinterpret_cast<void const*>(&_textureData);
}

/* IChild */
IParent* VulkanTexture::parent() const noexcept {
    return _heap->queryInterface<IParent>();
}

/* IInterface */
void* VulkanTexture::queryInterface(IID const& iid) noexcept {
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
    } else if (iid == ITexture::iid()) {
        return static_cast<ITexture*>(this);
    }

    return nullptr;
}

}

}
