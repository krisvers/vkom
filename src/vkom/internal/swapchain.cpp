#include <vkom/internal/swapchain.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/format.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

VulkanSwapchainMaintenance1PresentFence::VulkanSwapchainMaintenance1PresentFence(bool inheritedHandle, ISwapchain* swapchain, uint64_t presentID, VulkanFenceData const& fenceData, VulkanSwapchainData const& swapchainData) : _inheritedHandle(inheritedHandle), _swapchain(swapchain), _device(_swapchain->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _presentID(presentID), _fenceData(fenceData), _swapchainData(swapchainData) {
    _swapchain->retain();

    _device->label(this, fmt::label(_device, this).c_str());
}

VulkanSwapchainMaintenance1PresentFence::~VulkanSwapchainMaintenance1PresentFence() {
    _swapchain->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        _fenceData.deviceData.functionPointers.device10.vkDestroyFence(_fenceData.deviceData.vkDevice, _fenceData.vkFence, _fenceData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    _swapchain->release();
}

/* IPresentIDFence */
uint64_t VulkanSwapchainMaintenance1PresentFence::presentID() const noexcept {
    return _presentID;
}

/* IFence */
Result VulkanSwapchainMaintenance1PresentFence::wait(uint64_t timeout) noexcept {
    return castEnum<Result>(_fenceData.deviceData.functionPointers.device10.vkWaitForFences(_fenceData.deviceData.vkDevice, 1, &_fenceData.vkFence, true, timeout));
}

Result VulkanSwapchainMaintenance1PresentFence::reset() noexcept {
    return castEnum<Result>(_fenceData.deviceData.functionPointers.device10.vkResetFences(_fenceData.deviceData.vkDevice, 1, &_fenceData.vkFence));
}

bool VulkanSwapchainMaintenance1PresentFence::status() const noexcept {
    return (_fenceData.deviceData.functionPointers.device10.vkGetFenceStatus(_fenceData.deviceData.vkDevice, _fenceData.vkFence) == VK_SUCCESS);
}

/* IHandled */
uint64_t VulkanSwapchainMaintenance1PresentFence::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_fenceData.vkFence);
}

ObjectType VulkanSwapchainMaintenance1PresentFence::handleType() const noexcept {
    return ObjectType::Fence;
}

void const* VulkanSwapchainMaintenance1PresentFence::vkData() const noexcept {
    return reinterpret_cast<void const*>(&_fenceData);
}

/* IChild */
IParent* VulkanSwapchainMaintenance1PresentFence::parent() const noexcept {
    return _swapchain->queryInterface<IParent>();
}

/* IInterface */
void* VulkanSwapchainMaintenance1PresentFence::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IParent::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IFence::iid()) {
        return static_cast<IFence*>(this);
    } else if (iid == IPresentFence::iid()) {
        return static_cast<IPresentFence*>(this);
    } else if (iid == IPresentIDFence::iid()) {
        AdapterFeatures features;
        _adapter->queryFeatures(&features);

        if (features.presentID) {
            return static_cast<IPresentIDFence*>(this);
        }
    }

    return nullptr;
}

VulkanPresentWaitPresentIDFence::VulkanPresentWaitPresentIDFence(ISwapchain* swapchain, uint64_t presentID, VulkanSwapchainData const& swapchainData) : _swapchain(swapchain), _device(_swapchain->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _presentID(presentID), _swapchainData(swapchainData) {
    _swapchain->retain();

    _device->label(this, fmt::label(_device, this).c_str());
}

VulkanPresentWaitPresentIDFence::~VulkanPresentWaitPresentIDFence() {
    _swapchain->disown(IInterface::queryInterface<IChild>());
    _swapchain->release();
}

/* IPresentIDFence */
uint64_t VulkanPresentWaitPresentIDFence::presentID() const noexcept {
    return _presentID;
}

/* IFence */
Result VulkanPresentWaitPresentIDFence::wait(uint64_t timeout) noexcept {
    PFN_vkWaitForPresentKHR vkWaitForPresentKHR = _device->loadDispatchSymbol<PFN_vkWaitForPresentKHR>("vkWaitForPresentKHR");
    if (vkWaitForPresentKHR == nullptr) {
        return Result::ErrorUnsupportedFeature;
    }

    return castEnum<Result>(vkWaitForPresentKHR(_swapchainData.deviceData.vkDevice, _swapchainData.vkSwapchain, _presentID, timeout));
}

Result VulkanPresentWaitPresentIDFence::reset() noexcept {
    return Result::ErrorUnsupportedFeature;
}

bool VulkanPresentWaitPresentIDFence::status() const noexcept {
    PFN_vkWaitForPresentKHR vkWaitForPresentKHR = _device->loadDispatchSymbol<PFN_vkWaitForPresentKHR>("vkWaitForPresentKHR");
    if (vkWaitForPresentKHR == nullptr) {
        return false;
    }

    Result result = castEnum<Result>(vkWaitForPresentKHR(_swapchainData.deviceData.vkDevice, _swapchainData.vkSwapchain, _presentID, _defaultStatusTimeout));
    return (result == Result::Success || result == Result::SuboptimalSwapchain || result == Result::ErrorOutOfDateSwapchain);
}

/* IHandled */
uint64_t VulkanPresentWaitPresentIDFence::handle() const noexcept {
    return _presentID;
}

ObjectType VulkanPresentWaitPresentIDFence::handleType() const noexcept {
    return ObjectType::Unknown;
}

void const* VulkanPresentWaitPresentIDFence::vkData() const noexcept {
    return nullptr;
}

/* IChild */
IParent* VulkanPresentWaitPresentIDFence::parent() const noexcept {
    return _swapchain->queryInterface<IParent>();
}

/* IInterface */
void* VulkanPresentWaitPresentIDFence::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IParent::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IFence::iid()) {
        return static_cast<IFence*>(this);
    } else if (iid == IPresentFence::iid()) {
        return static_cast<IPresentFence*>(this);
    } else if (iid == IPresentIDFence::iid()) {
        return static_cast<IPresentIDFence*>(this);
    }

    return nullptr;
}

VulkanManualBackbuffer::VulkanManualBackbuffer(bool inheritedHandle, ISwapchain* swapchain, TextureInfo const& info, uint32_t index, VulkanTextureData const& textureData, VulkanSemaphoreData const& semaphoreData, VulkanSwapchainData const& swapchainData) : _inheritedHandle(inheritedHandle), _swapchain(swapchain), _device(_swapchain->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _info(info), _index(index), _textureData(textureData), _semaphoreData(semaphoreData), _swapchainData(swapchainData) {
    _device->label(this, fmt::label(_device, this).c_str());
}

VulkanManualBackbuffer::~VulkanManualBackbuffer() {
    ParentByVector::disownAll();
    _swapchain->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        _semaphoreData.deviceData.functionPointers.device10.vkDestroySemaphore(_semaphoreData.deviceData.vkDevice, _semaphoreData.vkSemaphore, _semaphoreData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    if (_acquired && !_presented) {
        releaseSwapchainImage();
    }
}

/* IBackbuffer */
uint32_t VulkanManualBackbuffer::index() const noexcept {
    return _index;
}

/* ITexture */
void VulkanManualBackbuffer::getInfo(TextureInfo* info) const noexcept {
    *info = _info;
}

Result VulkanManualBackbuffer::createView(TextureViewInfo const* info, ITextureView** view) noexcept {
    /* TODO: */
    return Result::ErrorUnknown;
}

/* IResource */
bool VulkanManualBackbuffer::isAlias() const noexcept {
    return false;
}

void VulkanManualBackbuffer::getAllocationInfo(ResourceAllocationInfo* info) const noexcept {
    info->alignment = 0;
    info->resourceSize = 0;
    info->allocationLocalOffset = 0;
    info->allocationLocalSize = 0;
}

void* VulkanManualBackbuffer::map() noexcept {
    return nullptr;
}

void VulkanManualBackbuffer::unmap() noexcept {
    return;
}

/* IHandled */
uint64_t VulkanManualBackbuffer::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_textureData.vkImage);
}

ObjectType VulkanManualBackbuffer::handleType() const noexcept {
    return ObjectType::Image;
}

void const* VulkanManualBackbuffer::vkData() const noexcept {
    return &_textureData;
}

/* IChild */
IParent* VulkanManualBackbuffer::parent() const noexcept {
    return _swapchain->queryInterface<IParent>();
}

/* ICollected */
uint32_t VulkanManualBackbuffer::release() {
    if (_referenceCount == 0) {
        return 0;
    }

    _referenceCount -= 1;
    if (_referenceCount == 0) {
        if (!_presented) {
            releaseSwapchainImage();
        }

        _acquired = false;
        _presented = false;
        return 0;
    }

    return _referenceCount;
}

uint32_t VulkanManualBackbuffer::retain() {
    _referenceCount += 1;
    return _referenceCount;
}

/* IInterface */
void* VulkanManualBackbuffer::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IParent::iid()) {
        return static_cast<IParent*>(this);
    } else if (iid == IResource::iid()) {
        return static_cast<IResource*>(this);
    } else if (iid == ITexture::iid()) {
        return static_cast<ITexture*>(this);
    } else if (iid == IBackbuffer::iid()) {
        return static_cast<IBackbuffer*>(this);
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

/* internal */
void VulkanManualBackbuffer::releaseSwapchainImage() noexcept {
    PFN_vkReleaseSwapchainImagesKHR vkReleaseSwapchainImagesKHR = _device->loadDispatchSymbol<PFN_vkReleaseSwapchainImagesKHR>("vkReleaseSwapchainImagesKHR");
    if (vkReleaseSwapchainImagesKHR == nullptr) {
        return;
    }

    VkReleaseSwapchainImagesInfoKHR releaseInfo = {};
    releaseInfo.sType = VK_STRUCTURE_TYPE_RELEASE_SWAPCHAIN_IMAGES_INFO_KHR;
    releaseInfo.swapchain = _swapchainData.vkSwapchain;
    releaseInfo.imageIndexCount = 1;
    releaseInfo.pImageIndices = &_index;

    vkReleaseSwapchainImagesKHR(_swapchainData.deviceData.vkDevice, &releaseInfo);
}

void VulkanManualBackbuffer::recreate(TextureInfo const& info, uint32_t index, VkImage vkImage) noexcept {
    if (_acquired && !_presented) {
        releaseSwapchainImage();
    }

    _referenceCount = 0;

    _info = info;
    _index = index;
    _textureData.vkImage = vkImage;
}

void VulkanManualBackbuffer::acquire() noexcept {
    if (_referenceCount != 0) {
        /* TODO: warn about missing releases */
    }

    _referenceCount = 0;
    _acquired = true;
}

void VulkanManualBackbuffer::present() noexcept {
    _presented = true;
}

VkSemaphore VulkanManualBackbuffer::vkBinarySemaphore() const noexcept {
    return _semaphoreData.vkSemaphore;
}

VulkanManualSwapchain::VulkanManualSwapchain(bool inheritedHandle, IDevice* device, ISurface* surface, SwapchainInfo const& info, VulkanSwapchainData const& swapchainData) : _inheritedHandle(inheritedHandle), _device(device), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _surface(surface), _info(info), _swapchainData(swapchainData), _backbufferHeapData(VulkanHeapData(_swapchainData.deviceData, nullptr)) {
    /* TODO: backbuffer creation */
    uint32_t backbufferCount;
    PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR = _device->loadDispatchSymbol<PFN_vkGetSwapchainImagesKHR>("vkGetSwapchainImagesKHR");
    if (vkGetSwapchainImagesKHR == nullptr) {
        throw std::runtime_error("vkGetSwapchainImagesKHR failed");
    }

    if (vkGetSwapchainImagesKHR(_swapchainData.deviceData.vkDevice, _swapchainData.vkSwapchain, &backbufferCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("vkGetSwapchainImagesKHR failed");
    }

    _backbufferImages.resize(backbufferCount);
    if (vkGetSwapchainImagesKHR(_swapchainData.deviceData.vkDevice, _swapchainData.vkSwapchain, &backbufferCount, &_backbufferImages[0]) != VK_SUCCESS) {
        throw std::runtime_error("vkGetSwapchainImagesKHR failed");
    }

    SurfaceFormat surfaceFormat;
    _adapter->enumerateSurfaceFormatsByBits(surface, info.surfaceFormatBits, &surfaceFormat);

    TextureInfo backbufferInfo = info.backbufferInfo;
    backbufferInfo.format = surfaceFormat.format;
    backbufferInfo.samplesPerTexel = 1;
    backbufferInfo.location = MemoryLocationFlags::GPU;

    for (uint32_t i = 0; i < backbufferCount; i += 1) {
        VulkanTextureData textureData = VulkanTextureData(_backbufferHeapData, nullptr, {}, _backbufferImages[i]);
        VkSemaphoreCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkSemaphore vkSemaphore;
        if (_swapchainData.deviceData.functionPointers.device10.vkCreateSemaphore(_swapchainData.deviceData.vkDevice, &createInfo, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkSemaphore) != VK_SUCCESS) {
            /* TODO: cleanup */

        }

        VulkanSemaphoreData semaphoreData = VulkanSemaphoreData(_swapchainData.deviceData, VK_SEMAPHORE_TYPE_BINARY, vkSemaphore);
        VulkanManualBackbuffer* backbuffer = new VulkanManualBackbuffer(false, this, backbufferInfo, i, textureData, semaphoreData, _swapchainData);
        adopt(backbuffer);
    }

    _surface->retain();
    _device->retain();

    _device->label(_surface, fmt::label(_device, _surface).c_str());
    _device->label(this, fmt::label(_device, this, "{} {}x{} backbuffers, {} {}, {} ({})", info.backbufferCount, info.backbufferInfo.dimensions.extent.width, info.backbufferInfo.dimensions.extent.height, info.preTransform, info.compositeAlpha, info.clipped ? "clipped" : "unclipped", info.presentModeFlags).c_str());
}

VulkanManualSwapchain::~VulkanManualSwapchain() {
    for (size_t i = 0; i < _dummyWSISync.size(); i += 1) {
        releaseDummyWSISync(_dummyWSISync.begin() + i, true);
        i -= 1;
    }

    _device->waitIdle();
    ParentByVector::disownAll();
    _device->disown(IInterface::queryInterface<IChild>());

    PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR = _device->loadDispatchSymbol<PFN_vkDestroySwapchainKHR>("vkDestroySwapchainKHR");
    if (vkDestroySwapchainKHR != nullptr) {
        vkDestroySwapchainKHR(_swapchainData.deviceData.vkDevice, _swapchainData.vkSwapchain, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    _surface->release();
    _device->release();
}

/* ISwapchain */
void VulkanManualSwapchain::getInfo(SwapchainInfo* info) const noexcept {
    *info = _info;
}

ISurface* VulkanManualSwapchain::surface() const noexcept {
    return _surface;
}

IBackbuffer* VulkanManualSwapchain::enumerateBackbuffers(uint32_t id) const noexcept {
    return IParent::enumerateChildren<IBackbuffer>(id);
}

Result VulkanManualSwapchain::recreate(SwapchainInfo const* info) noexcept {
    if (info->backbufferInfo.dimensions.extent.width == 0 || info->backbufferInfo.dimensions.extent.height == 0) {
        _lastRecreateMinimized = true;
        return Result::Success;
    }

    _lastRecreateMinimized = false;

    /* TODO: more intelligent surface format selection */
    SurfaceFormat surfaceFormat;
    uint32_t surfaceFormatIndex = std::numeric_limits<uint32_t>::max();
    for (uint32_t index = 0; index < 64; index += 1) {
        if ((info->surfaceFormatBits & (1 << index)) != 0) {
            if (!_adapter->enumerateSurfaceFormats(_surface, index, &surfaceFormat)) {
                continue;
            }

            surfaceFormatIndex = index;
            break;
        }
    }

    if (surfaceFormatIndex == std::numeric_limits<uint32_t>::max()) {
        return Result::ErrorUnknown;
    }

    AdapterLimits limits;
    _adapter->queryLimits(&limits);

    std::vector<uint32_t> indices(limits.queueFamilyCount);

    SwapchainInfo actualInfo = *info;
    actualInfo.preTransform = lowestFlag(info->preTransform);
    actualInfo.compositeAlpha = lowestFlag(info->compositeAlpha);
    actualInfo.surfaceFormatBits = (1 << surfaceFormatIndex);
    actualInfo.presentModeFlags = lowestFlag(info->presentModeFlags);

    /* TODO: better flag selection */
    VkSwapchainCreateInfoKHR vkCreateInfo = {};
    vkCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    vkCreateInfo.surface = _surface->handle<VkSurfaceKHR>();
    vkCreateInfo.minImageCount = info->backbufferCount;
    vkCreateInfo.imageFormat = castEnum<VkFormat>(surfaceFormat.format);
    vkCreateInfo.imageColorSpace = castEnum<VkColorSpaceKHR>(surfaceFormat.colorSpaceFlags);
    vkCreateInfo.imageExtent.width = info->backbufferInfo.dimensions.extent.width;
    vkCreateInfo.imageExtent.height = info->backbufferInfo.dimensions.extent.height;
    vkCreateInfo.imageArrayLayers = info->backbufferInfo.dimensions.subresource.layers;
    vkCreateInfo.imageUsage = castEnum<VkImageUsageFlags>(info->backbufferInfo.usage);
    vkCreateInfo.imageSharingMode = (info->backbufferInfo.queueConcurrency ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE);
    if (!info->backbufferInfo.queueConcurrency) {
        /* TODO: possibly more guaranteeable listing of all supported queue families? */
        for (uint32_t i = 0; i < limits.queueFamilyCount; i += 1) {
            indices[i] = i;
        }

        vkCreateInfo.queueFamilyIndexCount = limits.queueFamilyCount;
        vkCreateInfo.pQueueFamilyIndices = &indices[0];
    }

    vkCreateInfo.preTransform = castEnum<VkSurfaceTransformFlagBitsKHR>(actualInfo.preTransform);
    vkCreateInfo.compositeAlpha = castEnum<VkCompositeAlphaFlagBitsKHR>(actualInfo.compositeAlpha);
    vkCreateInfo.presentMode = castEnum<VkPresentModeKHR>(actualInfo.presentModeFlags);
    vkCreateInfo.clipped = info->clipped;
    vkCreateInfo.oldSwapchain = _swapchainData.vkSwapchain;

    PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR = _device->loadDispatchSymbol<PFN_vkCreateSwapchainKHR>("vkCreateSwapchainKHR");
    PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR = _device->loadDispatchSymbol<PFN_vkDestroySwapchainKHR>("vkDestroySwapchainKHR");
    if (vkCreateSwapchainKHR == nullptr || vkDestroySwapchainKHR == nullptr) {
        return Result::ErrorUnknown;
    }

    VkSwapchainKHR vkNewSwapchain;
    Result result = castEnum<Result>(vkCreateSwapchainKHR(_swapchainData.deviceData.vkDevice, &vkCreateInfo, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkNewSwapchain));
    if (result != Result::Success) {
        return result;
    }

    for (DummyWSISynchronizationPrimitives const& primitives : _dummyWSISync) {
        releaseDummyWSISync(primitives, true);
    }

    _dummyWSISync.clear();

    _device->waitIdle();
    vkDestroySwapchainKHR(_swapchainData.deviceData.vkDevice, _swapchainData.vkSwapchain, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks);

    uint32_t previousBackbufferCount = _swapchainData.backbufferCount;
    _swapchainData.backbufferCount = info->backbufferCount;
    _swapchainData.vkSwapchain = vkNewSwapchain;

    _info = actualInfo;

    uint32_t backbufferCount;
    PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR = _device->loadDispatchSymbol<PFN_vkGetSwapchainImagesKHR>("vkGetSwapchainImagesKHR");
    if (vkGetSwapchainImagesKHR == nullptr) {
        /* TODO: cleanup */
        return Result::ErrorUnknown;
    }

    if (vkGetSwapchainImagesKHR(_swapchainData.deviceData.vkDevice, _swapchainData.vkSwapchain, &backbufferCount, nullptr) != VK_SUCCESS) {
        /* TODO: cleanup */
        return Result::ErrorUnknown;
    }

    _backbufferImages.resize(backbufferCount);
    if (vkGetSwapchainImagesKHR(_swapchainData.deviceData.vkDevice, _swapchainData.vkSwapchain, &backbufferCount, &_backbufferImages[0]) != VK_SUCCESS) {
        /* TODO: cleanup */
        return Result::ErrorUnknown;
    }

    TextureInfo backbufferInfo = _info.backbufferInfo;
    backbufferInfo.format = surfaceFormat.format;
    backbufferInfo.samplesPerTexel = 1;
    backbufferInfo.location = MemoryLocationFlags::GPU;

    for (uint32_t i = 0; i < std::max(_swapchainData.backbufferCount, previousBackbufferCount); i += 1) {
        if (i >= info->backbufferCount) {
            VulkanManualBackbuffer* manualBackbuffer = dynamic_cast<VulkanManualBackbuffer*>(enumerateBackbuffers(i));
            delete manualBackbuffer;
        } else if (i >= previousBackbufferCount) {
            VulkanTextureData textureData = VulkanTextureData(_backbufferHeapData, nullptr, {}, _backbufferImages[i]);
            VkSemaphoreCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkSemaphore vkSemaphore;
            if (_swapchainData.deviceData.functionPointers.device10.vkCreateSemaphore(_swapchainData.deviceData.vkDevice, &createInfo, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkSemaphore) != VK_SUCCESS) {
                /* TODO: cleanup */
                return Result::ErrorUnknown;
            }

            VulkanSemaphoreData semaphoreData = VulkanSemaphoreData(_swapchainData.deviceData, VK_SEMAPHORE_TYPE_BINARY, vkSemaphore);
            VulkanManualBackbuffer* manualBackbuffer = new VulkanManualBackbuffer(false, this, backbufferInfo, i, textureData, semaphoreData, _swapchainData);
            adopt(manualBackbuffer);
        } else {
            VulkanManualBackbuffer* manualBackbuffer = dynamic_cast<VulkanManualBackbuffer*>(enumerateBackbuffers(i));
            manualBackbuffer->recreate(backbufferInfo, i, _backbufferImages[i]);
        }
    }

    return result;
}

Result VulkanManualSwapchain::acquireNextIndex(SemaphorePoint const* signalSemaphore, IFence* signalFence, uint32_t* index, uint64_t timeout) noexcept {
    for (size_t i = 0; i < _dummyWSISync.size(); i += 1) {
        if (_swapchainData.deviceData.functionPointers.device10.vkGetFenceStatus(_swapchainData.deviceData.vkDevice, _dummyWSISync[i].vkSemaphoreConsumedFence) == VK_SUCCESS) {
            releaseDummyWSISync(_dummyWSISync.begin() + i, false);
            i -= 1;
        }
    }

    bool timeline = false;
    uint64_t vkSemaphoreValue = 0;
    VkSemaphore vkProvidedSemaphore = VK_NULL_HANDLE;
    if (signalSemaphore != nullptr) {
        if (signalSemaphore->semaphore->handleType() != ObjectType::Semaphore) {
            return Result::ErrorUnknown;
        }

        vkProvidedSemaphore = signalSemaphore->semaphore->handle<VkSemaphore>();
        vkSemaphoreValue = signalSemaphore->value;

        timeline = (signalSemaphore->semaphore->queryInterface<ITimelineSemaphore>() != nullptr);
    }

    VkFence vkFence = VK_NULL_HANDLE;
    if (signalFence != nullptr) {
        if (signalFence->handleType() != ObjectType::Fence) {
            return Result::ErrorUnknown;
        }

        vkFence = signalFence->handle<VkFence>();
    }

    PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR = _device->loadDispatchSymbol<PFN_vkAcquireNextImageKHR>("vkAcquireNextImageKHR");
    if (vkAcquireNextImageKHR == nullptr) {
        return Result::ErrorUnsupportedFeature;
    }

    VkSemaphore vkBinarySemaphore = vkProvidedSemaphore;
    DummyWSISynchronizationPrimitives dummyPrimitives = {};
    if (timeline) {
        if (!acquireDummyWSISync(dummyPrimitives)) {
            return Result::ErrorUnsupportedFeature;
        }

        vkBinarySemaphore = dummyPrimitives.vkBinarySemaphore;
    }

    Result result = castEnum<Result>(vkAcquireNextImageKHR(_swapchainData.deviceData.vkDevice, _swapchainData.vkSwapchain, timeout, vkBinarySemaphore, vkFence, index));
    if (result != Result::Success && result != Result::NotReady && result != Result::SuboptimalSwapchain && result != Result::Timeout) {
        if (timeline) {
            releaseDummyWSISync(dummyPrimitives, false);
        }

        return result;
    }

    /* NOTE: this is kinda gross and should be avoided, but is relatively neglible in this case
    *   (implementations usually should not touch other implementation internals, even if its a child)
    *   (this is due to trying to allow for extensibility with adopting external implementations)
    */

    VulkanManualBackbuffer* manualBackbuffer = dynamic_cast<VulkanManualBackbuffer*>(enumerateBackbuffers(*index));
    manualBackbuffer->acquire();

    if (!timeline) {
        return result;
    }

    Result queueResult = dummySubmit({ dummyPrimitives.vkBinarySemaphore }, { 0 }, { VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT }, { vkProvidedSemaphore }, { vkSemaphoreValue }, dummyPrimitives.vkSemaphoreConsumedFence);
    if (queueResult != Result::Success) {
        releaseDummyWSISync(dummyPrimitives, false);
        return queueResult;
    }

    appendDummyWSISync(dummyPrimitives);
    return result;
}

Result VulkanManualSwapchain::present(IQueue* queue, IBackbuffer* backbuffer, PresentInfo const* info, IPresentFence** signal) noexcept {
    if (!hasChild(backbuffer)) {
        /* TODO: error */
        return Result::ErrorUnknown;
    }

    /* NOTE: this is kinda gross and should be avoided, but is relatively neglible in this case
    *   (implementations usually should not touch other implementation internals, even if its a child)
    *   (this is due to trying to allow for extensibility with adopting external implementations)
    */

    VulkanManualBackbuffer* manualBackbuffer = dynamic_cast<VulkanManualBackbuffer*>(backbuffer);

    PFN_vkQueuePresentKHR vkQueuePresentKHR = _device->loadDispatchSymbol<PFN_vkQueuePresentKHR>("vkQueuePresentKHR");
    if (vkQueuePresentKHR == nullptr) {
        /* TODO: error */
        return Result::ErrorUnsupportedFeature;
    }

    if ((queue->flags() & QueueFlags::Present) == QueueFlags::None) {
        /* TODO: warn about queue not advertising present support */
    }

    if (queue->handleType() != ObjectType::Queue) {
        /* TODO: error */
        return Result::ErrorUnknown;
    }

    VkQueue vkQueue = queue->handle<VkQueue>();

    VkSwapchainPresentFenceInfoKHR presentFenceInfo = {};
    presentFenceInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_PRESENT_FENCE_INFO_KHR;
    presentFenceInfo.swapchainCount = 1;

    uint64_t presentID = std::numeric_limits<uint64_t>::max();

    VkPresentIdKHR presentIdInfo = {};
    presentIdInfo.sType = VK_STRUCTURE_TYPE_PRESENT_ID_KHR;
    presentIdInfo.swapchainCount = 1;
    presentIdInfo.pPresentIds = &presentID;

    uint32_t backbufferIndex = backbuffer->index();

    std::vector<VkSemaphore> vkWaitSemaphores(info->waitCount);
    std::vector<uint64_t> vkWaitSemaphoreValues(info->waitCount);

    bool timeline = false;
    for (uint32_t i = 0; i < info->waitCount; i += 1) {
        if (info->waits[i].semaphore->queryInterface<ITimelineSemaphore>() != nullptr) {
            timeline = true;
        }

        vkWaitSemaphores[i] = info->waits[i].semaphore->handle<VkSemaphore>();
        vkWaitSemaphoreValues[i] = info->waits[i].value;
    }

    VkSemaphore vkBackbufferBinarySemaphore = manualBackbuffer->vkBinarySemaphore();

    std::vector<VkPipelineStageFlags> vkWaitDstStageMasks(info->waitCount, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
    Result result = dummySubmit(vkWaitSemaphores, vkWaitSemaphoreValues, vkWaitDstStageMasks, { vkBackbufferBinarySemaphore }, { 0 }, VK_NULL_HANDLE);
    if (result != Result::Success) {
        /* TODO: error */
        return result;
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &vkBackbufferBinarySemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &_swapchainData.vkSwapchain;
    presentInfo.pImageIndices = &backbufferIndex;

    AdapterFeatures features;
    _adapter->queryFeatures(&features);

    VkFence vkPresentFence = VK_NULL_HANDLE;
    IPresentFence* presentFence = nullptr;
    if (signal != nullptr) {
        if (features.swapchainMaintenance1) {
            presentFence = acquireSwapchainMaintenance1PresentFence(presentID);
            if (presentFence == nullptr) {
                /* TODO: error */
                return Result::ErrorUnknown;
            }

            vkPresentFence = presentFence->handle<VkFence>();
            presentFenceInfo.pFences = &vkPresentFence;

            presentFenceInfo.pNext = presentInfo.pNext;
            presentInfo.pNext = &presentFenceInfo;
        }
        else if (features.presentID && features.presentWait) {
            presentIdInfo.pNext = presentInfo.pNext;
            presentInfo.pNext = &presentIdInfo;
        }
    }

    result = castEnum<Result>(vkQueuePresentKHR(vkQueue, &presentInfo));
    if (result == Result::Success || result == Result::SuboptimalSwapchain) {
        *signal = presentFence;
        manualBackbuffer->present();
    } else {
        if (presentFence != nullptr) {
            if (result == Result::ErrorOutOfDateSwapchain) {
                presentFence->wait();
            }

            presentFence->release();
        }

        *signal = nullptr;
    }

    if (signal != nullptr && !features.swapchainMaintenance1 && features.presentID && features.presentWait) {
        presentFence = acquirePresentWaitPresentIDFence(presentID);
        if (presentFence == nullptr) {
            /* TODO: error */
            return Result::ErrorUnknown;
        }
    }

    _lastPresentQueue = vkQueue;

    return result;
}

/* IHandled */
uint64_t VulkanManualSwapchain::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_swapchainData.vkSwapchain);
}

ObjectType VulkanManualSwapchain::handleType() const noexcept {
    return ObjectType::SwapchainKHR;
}

void const* VulkanManualSwapchain::vkData() const noexcept {
    return &_swapchainData;
}

/* IChild */
IParent* VulkanManualSwapchain::parent() const noexcept {
    return _device->queryInterface<IParent>();
}

/* IInterface */
void* VulkanManualSwapchain::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IParent::iid()) {
        return static_cast<IParent*>(this);
    } else if (iid == ISwapchain::iid()) {
        return static_cast<ISwapchain*>(this);
    }

    return nullptr;
}

/* internal */
Result VulkanManualSwapchain::dummySubmit(std::vector<VkSemaphore> const& vkWaitSemaphores, std::vector<uint64_t> const& vkWaitSemaphoreValues, std::vector<VkPipelineStageFlags> const& vkWaitDstStageMasks, std::vector<VkSemaphore> const& vkSignalSemaphores, std::vector<uint64_t> const& vkSignalSemaphoreValues, VkFence vkSignalFence) noexcept {
    VkTimelineSemaphoreSubmitInfo timelineSubmitInfo = {};
    timelineSubmitInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
    timelineSubmitInfo.waitSemaphoreValueCount = static_cast<uint32_t>(vkWaitSemaphoreValues.size());
    timelineSubmitInfo.pWaitSemaphoreValues = (vkWaitSemaphoreValues.empty() ? nullptr : &vkWaitSemaphoreValues[0]);
    timelineSubmitInfo.signalSemaphoreValueCount = static_cast<uint32_t>(vkSignalSemaphoreValues.size());
    timelineSubmitInfo.pSignalSemaphoreValues = (vkSignalSemaphoreValues.empty() ? nullptr : &vkSignalSemaphoreValues[0]);

    AdapterFeatures adapterFeatures;
    _adapter->queryFeatures(&adapterFeatures);

    VkSubmitInfo dummySubmitInfo = {};
    dummySubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    if (adapterFeatures.timelineSemaphores) {
        dummySubmitInfo.pNext = &timelineSubmitInfo;
    }

    dummySubmitInfo.waitSemaphoreCount = static_cast<uint32_t>(vkWaitSemaphores.size());
    dummySubmitInfo.pWaitSemaphores = (vkWaitSemaphores.empty() ? nullptr : &vkWaitSemaphores[0]);
    dummySubmitInfo.pWaitDstStageMask = (vkWaitDstStageMasks.empty() ? nullptr : &vkWaitDstStageMasks[0]);
    dummySubmitInfo.signalSemaphoreCount = static_cast<uint32_t>(vkSignalSemaphores.size());
    dummySubmitInfo.pSignalSemaphores = (vkSignalSemaphores.empty() ? nullptr : &vkSignalSemaphores[0]);

    PFN_vkQueueSubmit vkQueueSubmit = _device->loadDispatchSymbol<PFN_vkQueueSubmit>("vkQueueSubmit");
    if (vkQueueSubmit == nullptr) {
        return Result::ErrorUnknown;
    }

    AdapterLimits adapterLimits;
    _adapter->queryLimits(&adapterLimits);

    VkQueue vkQueue = _lastPresentQueue;
    if (vkQueue == VK_NULL_HANDLE) {
        uint32_t firstPresentQueueFamily = std::numeric_limits<uint32_t>::max();
        for (uint32_t i = 0; i < adapterLimits.queueFamilyCount; i += 1) {
            if ((_adapter->queryQueueFamilyFlags(i) & QueueFlags::Present) != QueueFlags::None) {
                firstPresentQueueFamily = i;
                break;
            }
        }

        if (firstPresentQueueFamily == std::numeric_limits<uint32_t>::max()) {
            return Result::ErrorUnsupportedFeature;
        }

        _swapchainData.deviceData.functionPointers.device10.vkGetDeviceQueue(_swapchainData.deviceData.vkDevice, firstPresentQueueFamily, 0, &vkQueue);
    }

    PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT = _device->loadDispatchSymbol<PFN_vkQueueInsertDebugUtilsLabelEXT>("vkQueueInsertDebugUtilsLabelEXT");
    if (_swapchainData.deviceData.adapterData.instanceData.debug && vkQueueInsertDebugUtilsLabelEXT != nullptr) {
        VkDebugUtilsLabelEXT labelInfo = {};
        labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
        labelInfo.pLabelName = "WSI Dummy Timeline-Binary Submission";
        labelInfo.color[0] = 0.25f;
        labelInfo.color[1] = 0.25f;
        labelInfo.color[2] = 0.25f;
        labelInfo.color[3] = 1.00f;

        vkQueueInsertDebugUtilsLabelEXT(vkQueue, &labelInfo);
    }

    return castEnum<Result>(vkQueueSubmit(vkQueue, 1, &dummySubmitInfo, vkSignalFence));
}

VulkanSwapchainMaintenance1PresentFence* VulkanManualSwapchain::acquireSwapchainMaintenance1PresentFence(uint64_t presentID) noexcept {
    VkFenceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    VkFence vkFence;
    if (_swapchainData.deviceData.functionPointers.device10.vkCreateFence(_swapchainData.deviceData.vkDevice, &createInfo, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks, &vkFence) != VK_SUCCESS) {
        return nullptr;
    }

    VulkanSwapchainMaintenance1PresentFence* fence;
    VulkanFenceData fenceData = VulkanFenceData(_swapchainData.deviceData, false, vkFence);

    try {
        fence = new VulkanSwapchainMaintenance1PresentFence(false, this, presentID, fenceData, _swapchainData);
    } catch (std::runtime_error err) {
        return nullptr;
    }

    adopt(fence);
    _device->label(fence, "VulkanSwapchainMaintenance1PresentFence");
    return fence;
}

VulkanPresentWaitPresentIDFence* VulkanManualSwapchain::acquirePresentWaitPresentIDFence(uint64_t presentID) noexcept {
    VulkanPresentWaitPresentIDFence* fence;

    try {
        fence = new VulkanPresentWaitPresentIDFence(this, presentID, _swapchainData);
    } catch (std::runtime_error err) {
        return nullptr;
    }

    adopt(fence);
    return fence;
}

bool VulkanManualSwapchain::acquireDummyWSISync(DummyWSISynchronizationPrimitives& primitives) noexcept {
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (_swapchainData.deviceData.functionPointers.device10.vkCreateSemaphore(_swapchainData.deviceData.vkDevice, &semaphoreCreateInfo, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks, &primitives.vkBinarySemaphore) != VK_SUCCESS) {
        return false;
    }

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    if (_swapchainData.deviceData.functionPointers.device10.vkCreateFence(_swapchainData.deviceData.vkDevice, &fenceCreateInfo, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks, &primitives.vkSemaphoreConsumedFence) != VK_SUCCESS) {
        _swapchainData.deviceData.functionPointers.device10.vkDestroySemaphore(_swapchainData.deviceData.vkDevice, primitives.vkBinarySemaphore, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
        return false;
    }

    _device->labelHandle(ObjectType::Semaphore, primitives.vkBinarySemaphore, "WSI Dummy Binary Semaphore");
    _device->labelHandle(ObjectType::Fence, primitives.vkSemaphoreConsumedFence, "WSI Dummy Binary Semaphore Consumed Fence");

    return true;
}

void VulkanManualSwapchain::appendDummyWSISync(DummyWSISynchronizationPrimitives& primitives) noexcept {
    _dummyWSISync.push_back(primitives);
}

void VulkanManualSwapchain::releaseDummyWSISync(std::vector<DummyWSISynchronizationPrimitives>::iterator it, bool waitForConsumed) noexcept {
    releaseDummyWSISync(*it, waitForConsumed);
    _dummyWSISync.erase(it);
}

void VulkanManualSwapchain::releaseDummyWSISync(DummyWSISynchronizationPrimitives const& primitives, bool waitForConsumed) noexcept {
    if (waitForConsumed) {
        _swapchainData.deviceData.functionPointers.device10.vkWaitForFences(_swapchainData.deviceData.vkDevice, 1, &primitives.vkSemaphoreConsumedFence, true, _defaultTimeout);
    }

    _swapchainData.deviceData.functionPointers.device10.vkDestroyFence(_swapchainData.deviceData.vkDevice, primitives.vkSemaphoreConsumedFence, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    _swapchainData.deviceData.functionPointers.device10.vkDestroySemaphore(_swapchainData.deviceData.vkDevice, primitives.vkBinarySemaphore, _swapchainData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
}

}

}
