#include <vkom/internal/heap.hpp>

#include <stdexcept>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/resource.hpp>
#include <vkom/internal/buffer.hpp>
#include <vkom/internal/texture.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

namespace vkom {

namespace internal {

VulkanHeap::VulkanHeap(bool inheritedHandle, IDevice* device, VulkanHeapData const& heapData) : _inheritedHandle(inheritedHandle), _device(device), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _heapData(heapData) {
    _device->retain();
}

VulkanHeap::~VulkanHeap() {
    _device->waitIdle();
    ParentByVector::disownAll();
    _device->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle && _heapData.vmaPool != nullptr) {
        vmaDestroyPool(_heapData.deviceData.vmaAllocator, _heapData.vmaPool);
    }

    _device->release();
}

/* IHeap */
Result VulkanHeap::createBuffer(BufferInfo const* info, IBuffer** buffer) noexcept {
    /* TODO: info validation */

    AdapterLimits limits;
    _adapter->queryLimits(&limits);

    std::vector<uint32_t> indices(limits.queueFamilyCount);

    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = info->size;
    createInfo.usage = castEnum<VkBufferUsageFlags>(info->usage);
    createInfo.sharingMode = (info->queueConcurrency ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE);
    if (!info->queueConcurrency) {
        /* TODO: possibly more guaranteeable listing of all supported queue families? */
        for (uint32_t i = 0; i < limits.queueFamilyCount; i += 1) {
            indices[i] = i;
        }

        createInfo.queueFamilyIndexCount = limits.queueFamilyCount;
        createInfo.pQueueFamilyIndices = &indices[0];
    }

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.flags = ((info->location & MemoryLocationFlags::CPU) != MemoryLocationFlags::None ? VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT : 0);
    allocationCreateInfo.usage = ((info->location & MemoryLocationFlags::GPU) != MemoryLocationFlags::None) ? VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE : ((info->location & MemoryLocationFlags::CPU) != MemoryLocationFlags::None) ? VMA_MEMORY_USAGE_AUTO_PREFER_HOST : VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    allocationCreateInfo.preferredFlags = ((info->location & MemoryLocationFlags::GPU) != MemoryLocationFlags::None) ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : 0;
    allocationCreateInfo.pool = _heapData.vmaPool;

    VkBuffer vkBuffer;
    VmaAllocation vmaAllocation;
    Result result = castEnum<Result>(vmaCreateBuffer(_heapData.deviceData.vmaAllocator, &createInfo, &allocationCreateInfo, &vkBuffer, &vmaAllocation, nullptr));
    if (result != Result::Success) {
        return result;
    }

    VmaAllocationInfo2 vmaAllocationInfo2;
    vmaGetAllocationInfo2(_heapData.deviceData.vmaAllocator, vmaAllocation, &vmaAllocationInfo2);

    VulkanBufferData bufferData = VulkanBufferData(_heapData, vmaAllocation, vmaAllocationInfo2, vkBuffer);

    try {
        *buffer = new VulkanBuffer(false, false, this, *info, bufferData);
    } catch (std::runtime_error err) {
        vmaDestroyBuffer(_heapData.deviceData.vmaAllocator, vkBuffer, vmaAllocation);
        return Result::ErrorUnknown;
    }

    adopt(*buffer);
    return Result::Success;
}

Result VulkanHeap::createAliasedBuffer(BufferInfo const* info, ResourceAliasingInfo const* aliasingInfo, IBuffer** buffer) noexcept {
    /* TODO: info validation */

    VmaAllocation vmaAllocationToAlias;
    IBuffer* bufferToAlias = aliasingInfo->resource->queryInterface<IBuffer>();
    ITexture* textureToAlias = aliasingInfo->resource->queryInterface<ITexture>();
    if (textureToAlias != nullptr) {
        VulkanTextureData const* textureData = textureToAlias->vkData<VulkanTextureData>();
        if (textureData == nullptr) {
            /* TODO: error */
            return Result::ErrorUnknown;
        }

        vmaAllocationToAlias = textureData->vmaAllocation;
    }

    AdapterLimits limits;
    _adapter->queryLimits(&limits);

    std::vector<uint32_t> indices(limits.queueFamilyCount);

    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = info->size;
    createInfo.usage = castEnum<VkBufferUsageFlags>(info->usage);
    createInfo.sharingMode = (info->queueConcurrency ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE);
    if (!info->queueConcurrency) {
        /* TODO: possibly more guaranteeable listing of all supported queue families? */
        for (uint32_t i = 0; i < limits.queueFamilyCount; i += 1) {
            indices[i] = i;
        }

        createInfo.queueFamilyIndexCount = limits.queueFamilyCount;
        createInfo.pQueueFamilyIndices = &indices[0];
    }

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.flags = ((info->location & MemoryLocationFlags::CPU) != MemoryLocationFlags::None ? VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT : 0);
    allocationCreateInfo.usage = ((info->location & MemoryLocationFlags::GPU) != MemoryLocationFlags::None) ? VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE : ((info->location & MemoryLocationFlags::CPU) != MemoryLocationFlags::None) ? VMA_MEMORY_USAGE_AUTO_PREFER_HOST : VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    allocationCreateInfo.preferredFlags = ((info->location & MemoryLocationFlags::GPU) != MemoryLocationFlags::None) ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : 0;
    allocationCreateInfo.pool = _heapData.vmaPool;

    VkBuffer vkBuffer;
    VmaAllocation vmaAllocation;
    Result result = castEnum<Result>(vmaCreateAliasingBuffer2(_heapData.deviceData.vmaAllocator, &createInfo, &allocationCreateInfo, &vkBuffer, &vmaAllocation, nullptr));
    if (result != Result::Success) {
        return result;
    }

    VmaAllocationInfo2 vmaAllocationInfo2;
    vmaGetAllocationInfo2(_heapData.deviceData.vmaAllocator, vmaAllocation, &vmaAllocationInfo2);

    VulkanBufferData bufferData = VulkanBufferData(_heapData, vmaAllocation, vmaAllocationInfo2, vkBuffer);

    try {
        *buffer = new VulkanBuffer(false, true, this, *info, bufferData);
    } catch (std::runtime_error err) {
        vmaDestroyBuffer(_heapData.deviceData.vmaAllocator, vkBuffer, vmaAllocation);
        return Result::ErrorUnknown;
    }

    adopt(*buffer);
    return Result::Success;
}

Result VulkanHeap::createTexture(TextureInfo const* info, ITexture** texture) noexcept {AdapterLimits limits;
    /* TODO: info validation */

    _adapter->queryLimits(&limits);

    std::vector<uint32_t> indices(limits.queueFamilyCount);

    VkImageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    if (info->dimensions.extent.height == 0) {
        createInfo.imageType = VK_IMAGE_TYPE_1D;
    } else if (info->dimensions.extent.depth == 0) {
        createInfo.imageType = VK_IMAGE_TYPE_2D;
    } else {
        createInfo.imageType = VK_IMAGE_TYPE_3D;
    }

    createInfo.format = castEnum<VkFormat>(info->format);
    createInfo.extent.width = std::max(1u, info->dimensions.extent.width);
    createInfo.extent.height = std::max(1u, info->dimensions.extent.height);
    createInfo.extent.depth = std::max(1u, info->dimensions.extent.depth);
    createInfo.mipLevels = info->dimensions.subresource.mips;
    createInfo.arrayLayers = info->dimensions.subresource.layers;
    createInfo.samples = static_cast<VkSampleCountFlagBits>(info->samplesPerTexel);
    createInfo.tiling = (info->linearTiling ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL);
    createInfo.usage = castEnum<VkImageUsageFlags>(info->usage);
    createInfo.sharingMode = (info->queueConcurrency ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE);
    if (!info->queueConcurrency) {
        /* TODO: possibly more guaranteeable listing of all supported queue families? */
        for (uint32_t i = 0; i < limits.queueFamilyCount; i += 1) {
            indices[i] = i;
        }

        createInfo.queueFamilyIndexCount = limits.queueFamilyCount;
        createInfo.pQueueFamilyIndices = &indices[0];
    }

    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.flags = ((info->location & MemoryLocationFlags::CPU) != MemoryLocationFlags::None ? VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT : 0);
    allocationCreateInfo.usage = ((info->location & MemoryLocationFlags::GPU) != MemoryLocationFlags::None) ? VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE : ((info->location & MemoryLocationFlags::CPU) != MemoryLocationFlags::None) ? VMA_MEMORY_USAGE_AUTO_PREFER_HOST : VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    allocationCreateInfo.preferredFlags = ((info->location & MemoryLocationFlags::GPU) != MemoryLocationFlags::None) ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : 0;
    allocationCreateInfo.pool = _heapData.vmaPool;

    VkImage vkImage;
    VmaAllocation vmaAllocation;
    Result result = castEnum<Result>(vmaCreateImage(_heapData.deviceData.vmaAllocator, &createInfo, &allocationCreateInfo, &vkImage, &vmaAllocation, nullptr));
    if (result != Result::Success) {
        return result;
    }

    VmaAllocationInfo2 vmaAllocationInfo2;
    vmaGetAllocationInfo2(_heapData.deviceData.vmaAllocator, vmaAllocation, &vmaAllocationInfo2);

    VulkanTextureData textureData = VulkanTextureData(_heapData, vmaAllocation, vmaAllocationInfo2, vkImage);

    try {
        *texture = new VulkanTexture(false, this, textureData);
    } catch (std::runtime_error err) {
        vmaDestroyImage(_heapData.deviceData.vmaAllocator, vkImage, vmaAllocation);
        return Result::ErrorUnknown;
    }

    adopt(*texture);
    return Result::Success;
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

void const* VulkanHeap::vkData() const noexcept {
    return &_heapData;
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
