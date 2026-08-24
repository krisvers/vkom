#include <vkom/internal/texture.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/heap.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>
#include <vkom/internal/format.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vma.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

VulkanTextureView::VulkanTextureView(bool inheritedHandle, ITexture* texture, TextureViewInfo const& info, VulkanTextureViewData const& viewData) : _inheritedHandle(inheritedHandle), _texture(texture), _heap(_texture->parent<IHeap>()), _device(_heap->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _info(info), _viewData(viewData) {
    _texture->retain();

    _device->label(this, fmt::label(_device, this, "{}.{} {}.{}, {}, {}.{}.{}.{} ({}), ({})", info.subresourcePosition.layer, info.subresourcePosition.mip, info.subresourceDimensions.layers, info.subresourceDimensions.mips, info.type, info.redSwizzle, info.greenSwizzle, info.blueSwizzle, info.alphaSwizzle, info.aspectFlags, info.format).c_str());
}

VulkanTextureView::~VulkanTextureView() {
    _texture->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle && _viewData.vkImageView != VK_NULL_HANDLE) {
        _viewData.textureData.heapData.deviceData.functionPointers.device10.vkDestroyImageView(_viewData.textureData.heapData.deviceData.vkDevice, _viewData.vkImageView, _viewData.textureData.heapData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    _texture->release();
}

/* IBufferView */
void VulkanTextureView::getInfo(TextureViewInfo* info) const noexcept {
    *info = _info;
}

/* IHandled */
uint64_t VulkanTextureView::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_viewData.vkImageView);
}

ObjectType VulkanTextureView::handleType() const noexcept {
    return ObjectType::ImageView;
}

void const* VulkanTextureView::vkData() const noexcept {
    return &_viewData;
}

/* IChild */
IParent* VulkanTextureView::parent() const noexcept {
    return _texture->queryInterface<IParent>();
}

/* IInterface */
void* VulkanTextureView::queryInterface(IID const& iid) noexcept {
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
    } else if (iid == ITextureView::iid()) {
        return static_cast<ITextureView*>(this);
    }

    return nullptr;
}

VulkanTexture::VulkanTexture(bool inheritedHandle, bool alias, IHeap* heap, TextureInfo const& info, VulkanTextureData const& TextureData) : _inheritedHandle(inheritedHandle), _alias(alias), _heap(heap), _device(_heap->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _info(info), _textureData(TextureData) {
    _heap->retain();

    _device->label(this, fmt::label(_device, this, "{}x{}x{}:{}.{}, {}x sampling, {}, {} {} ({}) ({})", info.dimensions.extent.width, info.dimensions.extent.height, info.dimensions.extent.depth, info.dimensions.subresource.layers, info.dimensions.subresource.mips, info.samplesPerTexel, info.location, info.linearTiling ? "linear" : "optimal", info.queueConcurrency ? "concurrent" : "exclusive", info.format, info.usage).c_str());
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
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
    createInfo.image = _textureData.vkImage;
    createInfo.viewType = castEnum<VkImageViewType>(info->type);
    createInfo.format = castEnum<VkFormat>(info->format);
    createInfo.components.r = castEnum<VkComponentSwizzle>(info->redSwizzle);
    createInfo.components.g = castEnum<VkComponentSwizzle>(info->greenSwizzle);
    createInfo.components.b = castEnum<VkComponentSwizzle>(info->blueSwizzle);
    createInfo.components.a = castEnum<VkComponentSwizzle>(info->alphaSwizzle);
    createInfo.subresourceRange.aspectMask = castEnum<VkImageAspectFlags>(info->aspectFlags);
    createInfo.subresourceRange.baseMipLevel = info->subresourcePosition.mip;
    createInfo.subresourceRange.levelCount = info->subresourceDimensions.mips;
    createInfo.subresourceRange.baseArrayLayer = info->subresourcePosition.layer;
    createInfo.subresourceRange.layerCount = info->subresourceDimensions.layers;

    VkImageView vkImageView;
    Result result = castEnum<Result>(_textureData.heapData.deviceData.functionPointers.device10.vkCreateImageView(_textureData.heapData.deviceData.vkDevice, &createInfo, _textureData.heapData.deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkImageView));
    if (result != Result::Success) {
        return result;
    }

    VulkanTextureViewData viewData = VulkanTextureViewData(_textureData, vkImageView);

    try {
        *view = new VulkanTextureView(false, this, *info, viewData);
    } catch (std::runtime_error err) {
        /* TODO: error */
        _textureData.heapData.deviceData.functionPointers.device10.vkDestroyImageView(_textureData.heapData.deviceData.vkDevice, vkImageView, _textureData.heapData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
        return Result::ErrorUnknown;
    }

    adopt(*view);
    return Result::Success;
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
    } else if (iid == ITransferSourceTexture::iid()) {
        if ((_info.usage & TextureUsageFlags::TransferSource) != TextureUsageFlags::None) {
            return static_cast<ITransferSourceTexture*>(this);
        }

        return nullptr;
    } else if (iid == ITransferDestinationTexture::iid()) {
        if ((_info.usage & TextureUsageFlags::TransferDestination) != TextureUsageFlags::None) {
            return static_cast<ITransferDestinationTexture*>(this);
        }

        return nullptr;
    } else if (iid == ISampledTexture::iid()) {
        if ((_info.usage & TextureUsageFlags::Sampled) != TextureUsageFlags::None) {
            return static_cast<ISampledTexture*>(this);
        }

        return nullptr;
    } else if (iid == IStorageTexture::iid()) {
        if ((_info.usage & TextureUsageFlags::Storage) != TextureUsageFlags::None) {
            return static_cast<IStorageTexture*>(this);
        }

        return nullptr;
    } else if (iid == IRenderTarget::iid()) {
        if ((_info.usage & TextureUsageFlags::RenderTarget) != TextureUsageFlags::None) {
            return static_cast<IRenderTarget*>(this);
        }

        return nullptr;
    } else if (iid == IDepthStencilTarget::iid()) {
        if ((_info.usage & TextureUsageFlags::DepthStencilTarget) != TextureUsageFlags::None) {
            return static_cast<IDepthStencilTarget*>(this);
        }

        return nullptr;
    } else if (iid == ITransientTarget::iid()) {
        if ((_info.usage & TextureUsageFlags::TransientTarget) != TextureUsageFlags::None) {
            return static_cast<ITransientTarget*>(this);
        }

        return nullptr;
    } else if (iid == IInputTarget::iid()) {
        if ((_info.usage & TextureUsageFlags::InputTarget) != TextureUsageFlags::None) {
            return static_cast<IInputTarget*>(this);
        }

        return nullptr;
    }

    return nullptr;
}

}

}
