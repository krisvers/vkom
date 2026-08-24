#include <vkom/internal/descriptor.hpp>

#include <limits>
#include <stdexcept>
#include <algorithm>

#include <vkom/buffer.hpp>
#include <vkom/texture.hpp>

#include <vkom/internal/enums.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(bool inheritedHandle, IDevice* device, DescriptorSetLayoutInfo const& info, VulkanDescriptorSetLayoutData const& layoutData) : _inheritedHandle(inheritedHandle), _device(device), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _layoutData(layoutData) {
    _device->retain();

    _info.bindings.resize(info.bindingCount);
    for (uint32_t i = 0; i < info.bindingCount; i += 1) {
        size_t immutableSamplersStart = _info.bindingImmutableSamplers.size();
        for (uint32_t j = 0; j < info.bindings[i].count; j += 1) {
            if (info.bindings[i].immutableSamplers == nullptr) {
                break;
            }

            _info.bindingImmutableSamplers.push_back(info.bindings[i].immutableSamplers[j]);
        }

        _info.bindings[i] = info.bindings[i];
        _info.bindings[i].immutableSamplers = ((_info.bindingImmutableSamplers.size() == immutableSamplersStart) ? nullptr : &_info.bindingImmutableSamplers[immutableSamplersStart]);
    }

    std::sort(_info.bindings.begin(), _info.bindings.end(), [](DescriptorBindingInfo const& a, DescriptorBindingInfo const& b) -> bool {
        return b.binding > a.binding;
    });

    _info.info = info;
    _info.info.bindings = &_info.bindings[0];

    _device->label(this, fmt::label(_device, this, "{} bindings ({})", queryDescriptorBindingCount(), flags()).c_str());
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
    _device->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        _layoutData.deviceData.functionPointers.device10.vkDestroyDescriptorSetLayout(_layoutData.deviceData.vkDevice, _layoutData.vkDescriptorSetLayout, _layoutData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    _device->release();
}

/* IDescriptorSetLayout */
DescriptorSetLayoutFlags VulkanDescriptorSetLayout::flags() const noexcept {
    return _info.info.flags;
}

/* IDescriptorSetLayoutInfoQuery */
uint32_t VulkanDescriptorSetLayout::queryDescriptorBindingCount() const noexcept {
    return _info.info.bindingCount;
}

uint32_t VulkanDescriptorSetLayout::queryDescriptorBindingIDByIndex(uint32_t index) const noexcept {
    if (index > _info.info.bindingCount) {
        return std::numeric_limits<uint32_t>::max();
    }

    return _info.info.bindings[index].binding;
}

bool VulkanDescriptorSetLayout::queryDescriptorBindingInfo(uint32_t binding, DescriptorBindingInfo* info) const noexcept {
    for (uint32_t i = 0; i < _info.info.bindingCount; i += 1) {
        if (_info.info.bindings[i].binding == binding) {
            *info = _info.info.bindings[i];
            return true;
        }
    }

    return false;
}

uint32_t VulkanDescriptorSetLayout::queryDescriptorTypeCount(DescriptorFlags flags) const noexcept {
    for (uint32_t i = 0; i < _info.info.bindingCount; i += 1) {
        if (_info.info.bindings[i].flags == flags) {
            return _info.info.bindings[i].count;
        }
    }

    return 0;
}

/* IHandled */
uint64_t VulkanDescriptorSetLayout::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_layoutData.vkDescriptorSetLayout);
}

ObjectType VulkanDescriptorSetLayout::handleType() const noexcept {
    return ObjectType::DescriptorSetLayout;
}

void const* VulkanDescriptorSetLayout::vkData() const noexcept {
    return &_layoutData;
}

/* IChild */
IParent* VulkanDescriptorSetLayout::parent() const noexcept {
    return _device->queryInterface<IParent>();
}

/* IInterface */
void* VulkanDescriptorSetLayout::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IDescriptorSetLayoutInfoQuery::iid()) {
        return static_cast<IDescriptorSetLayoutInfoQuery*>(this);
    } else if (iid == IDescriptorSetLayout::iid()) {
        return static_cast<IDescriptorSetLayout*>(this);
    }

    return nullptr;
}

VulkanDescriptorSet::VulkanDescriptorSet(bool inheritedHandle, IDescriptorPool* pool, DescriptorSetLayoutInfo const& layoutInfo, VulkanDescriptorSetData const& setData) : _inheritedHandle(inheritedHandle), _pool(pool), _device(_pool->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _setData(setData) {
    _pool->retain();

    _layoutInfo.bindings.resize(layoutInfo.bindingCount);
    for (uint32_t i = 0; i < layoutInfo.bindingCount; i += 1) {
        size_t immutableSamplersStart = _layoutInfo.bindingImmutableSamplers.size();
        for (uint32_t j = 0; j < layoutInfo.bindings[i].count; j += 1) {
            if (layoutInfo.bindings[i].immutableSamplers == nullptr) {
                break;
            }

            _layoutInfo.bindingImmutableSamplers.push_back(layoutInfo.bindings[i].immutableSamplers[j]);
        }

        _layoutInfo.bindings[i] = layoutInfo.bindings[i];
        _layoutInfo.bindings[i].immutableSamplers = ((_layoutInfo.bindingImmutableSamplers.size() == immutableSamplersStart) ? nullptr : &_layoutInfo.bindingImmutableSamplers[immutableSamplersStart]);;
    }

    std::sort(_layoutInfo.bindings.begin(), _layoutInfo.bindings.end(), [](DescriptorBindingInfo const& a, DescriptorBindingInfo const& b) -> bool {
        return b.binding > a.binding;
    });

    _layoutInfo.info = layoutInfo;
    _layoutInfo.info.bindings = &_layoutInfo.bindings[0];

    _device->label(this, fmt::label(_device, this, "{} bindings ({})", queryDescriptorBindingCount(), poolFlags()).c_str());
}

VulkanDescriptorSet::~VulkanDescriptorSet() {
    _pool->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        if ((poolFlags() & DescriptorPoolFlags::FreeDescriptorSet) != DescriptorPoolFlags::None) {
            _setData.poolData.deviceData.functionPointers.device10.vkFreeDescriptorSets(_setData.poolData.deviceData.vkDevice, _setData.poolData.vkDescriptorPool, 1, &_setData.vkDescriptorSet);
        }
    }

    _pool->release();
}

/* IDescriptorSet */
DescriptorPoolFlags VulkanDescriptorSet::poolFlags() const noexcept {
    return _pool->flags();
}

void VulkanDescriptorSet::write(uint32_t writeCount, DescriptorWrite const* writes) noexcept {
    std::vector<VkDescriptorImageInfo> vkImageInfos = {};
    std::vector<VkDescriptorBufferInfo> vkBufferInfos = {};
    std::vector<VkBufferView> vkTexelBufferViews = {};

    std::vector<VkWriteDescriptorSet> vkWrites(writeCount);
    for (uint32_t i = 0; i < writeCount; i += 1) {
        DescriptorBindingInfo bindingInfo;
        if (!queryDescriptorBindingInfo(writes[i].binding, &bindingInfo)) {
            /* TODO: warning (discarded) */
            continue;
        }

        vkWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        vkWrites[i].dstSet = _setData.vkDescriptorSet;
        vkWrites[i].dstBinding = writes[i].binding;
        vkWrites[i].dstArrayElement = writes[i].element;
        vkWrites[i].descriptorCount = writes[i].count;
        vkWrites[i].descriptorType = castEnum<VkDescriptorType>(bindingInfo.flags);

        if ((bindingInfo.flags & DescriptorFlags::Texel) != DescriptorFlags::None) {
            size_t texelBufferViewsStart = vkTexelBufferViews.size();
            for (uint32_t j = 0; j < writes[i].count; j += 1) {
                IResourceView* view = writes[i].views[j];
                IBufferView* bufferView = view->queryInterface<IBufferView>();
                if (bufferView == nullptr) {
                    /* TODO: error */
                    return;
                }

                IBuffer* buffer = bufferView->parent<IBuffer>();
                if (buffer == nullptr) {
                    /* TODO: error */
                    return;
                }

                if ((bindingInfo.flags & DescriptorFlags::Uniform) != DescriptorFlags::None) {
                    IUniformTexelBuffer* uniformTexelBuffer = buffer->queryInterface<IUniformTexelBuffer>();
                    if (uniformTexelBuffer == nullptr) {
                        /* TODO: error */
                        return;
                    }
                } else if ((bindingInfo.flags & DescriptorFlags::Storage) != DescriptorFlags::None) {
                    IStorageTexelBuffer* storageTexelBuffer = buffer->queryInterface<IStorageTexelBuffer>();
                    if (storageTexelBuffer == nullptr) {
                        /* TODO: error */
                        return;
                    }
                }

                if (bufferView->handleType() != ObjectType::BufferView) {
                    /* TODO: error */
                    return;
                }

                VkBufferView vkBufferView = bufferView->handle<VkBufferView>();
                vkTexelBufferViews.push_back(vkBufferView);
            }

            vkWrites[i].pTexelBufferView = (vkTexelBufferViews.size() == texelBufferViewsStart ? nullptr : &vkTexelBufferViews[texelBufferViewsStart]);
        } else if ((bindingInfo.flags & DescriptorFlags::Texture) != DescriptorFlags::None) {
            size_t imageInfosStart = vkImageInfos.size();
            for (uint32_t j = 0; j < writes[i].count; j += 1) {
                VkSampler vkSampler = VK_NULL_HANDLE;
                if ((bindingInfo.flags & DescriptorFlags::Sampler) != DescriptorFlags::None) {
                    ISampler* sampler = writes[i].textureInfos[j].sampler;
                    if (sampler == nullptr) {
                        /* TODO: error */
                        return;
                    }

                    if (sampler->handleType() != ObjectType::Sampler) {
                        /* TODO: error */
                        return;
                    }

                    vkSampler = sampler->handle<VkSampler>();
                } else if (writes[i].textureInfos[j].sampler != nullptr) {
                    /* TODO: warn (unused sampler provided) */
                }

                IResourceView* view = writes[i].views[j];
                ITextureView* textureView = view->queryInterface<ITextureView>();
                if (textureView == nullptr) {
                    /* TODO: error */
                    return;
                }

                ITexture* texture = textureView->parent<ITexture>();
                if (texture == nullptr) {
                    /* TODO: error */
                    return;
                }

                if ((bindingInfo.flags & DescriptorFlags::Storage) != DescriptorFlags::None) {
                    IStorageTexture* storageTexture = texture->queryInterface<IStorageTexture>();
                    if (storageTexture == nullptr) {
                        /* TODO: error */
                        return;
                    }
                } else if ((bindingInfo.flags & DescriptorFlags::Sampled) != DescriptorFlags::None) {
                    ISampledTexture* sampledTexture = texture->queryInterface<ISampledTexture>();
                    if (sampledTexture == nullptr) {
                        /* TODO: error */
                        return;
                    }
                } else if ((bindingInfo.flags & DescriptorFlags::Input) != DescriptorFlags::None) {
                    IInputTarget* inputTarget = texture->queryInterface<IInputTarget>();
                    if (inputTarget == nullptr) {
                        /* TODO: error */
                        return;
                    }
                }

                if (textureView->handleType() != ObjectType::ImageView) {
                    /* TODO: error */
                    return;
                }

                VkImageView vkImageView = textureView->handle<VkImageView>();

                VkDescriptorImageInfo vkImageInfo = {};
                vkImageInfo.sampler = vkSampler;
                vkImageInfo.imageView = vkImageView;
                vkImageInfo.imageLayout = castEnum<VkImageLayout>(writes[i].textureInfos[j].layout);

                vkImageInfos.push_back(vkImageInfo);
            }

            vkWrites[i].pImageInfo = (vkImageInfos.size() == imageInfosStart ? nullptr : &vkImageInfos[imageInfosStart]);
        } else if ((bindingInfo.flags & DescriptorFlags::Buffer) != DescriptorFlags::None) {
            size_t bufferInfosStart = vkBufferInfos.size();
            for (uint32_t j = 0; j < writes[i].count; j += 1) {
                IResourceView* view = writes[i].views[j];
                if (view == nullptr) {
                    /* TODO: error */
                    return;
                }

                IBufferView* bufferView = view->queryInterface<IBufferView>();
                if (bufferView == nullptr) {
                    /* TODO: error */
                    return;
                }

                IBuffer* buffer = bufferView->parent<IBuffer>();
                if (buffer == nullptr) {
                    /* TODO: error */
                    return;
                }

                if ((bindingInfo.flags & DescriptorFlags::Uniform) != DescriptorFlags::None) {
                    IUniformBuffer* uniformBuffer = buffer->queryInterface<IUniformBuffer>();
                    if (uniformBuffer == nullptr) {
                        /* TODO: error */
                        return;
                    }
                } else if ((bindingInfo.flags & DescriptorFlags::Storage) != DescriptorFlags::None) {
                    IStorageBuffer* storageBuffer = buffer->queryInterface<IStorageBuffer>();
                    if (storageBuffer == nullptr) {
                        /* TODO: error */
                        return;
                    }
                }

                if (buffer->handleType() != ObjectType::Buffer) {
                    /* TODO: error */
                    return;
                }

                VkBuffer vkBuffer = buffer->handle<VkBuffer>();

                BufferViewInfo viewInfo;
                bufferView->getInfo(&viewInfo);

                VkDescriptorBufferInfo vkBufferInfo = {};
                vkBufferInfo.buffer = vkBuffer;
                vkBufferInfo.offset = static_cast<VkDeviceSize>(viewInfo.offset);
                vkBufferInfo.range = static_cast<VkDeviceSize>(viewInfo.range);

                vkBufferInfos.push_back(vkBufferInfo);
            }

            vkWrites[i].pBufferInfo = (vkBufferInfos.size() == bufferInfosStart ? nullptr : &vkBufferInfos[bufferInfosStart]);
        }
    }

    _setData.poolData.deviceData.functionPointers.device10.vkUpdateDescriptorSets(_setData.poolData.deviceData.vkDevice, static_cast<uint32_t>(vkWrites.size()), vkWrites.empty() ? nullptr : &vkWrites[0], 0, nullptr);
}

void VulkanDescriptorSet::copy(uint32_t copyCount, DescriptorCopy const* copies) noexcept {
    std::vector<VkCopyDescriptorSet> vkCopies(copyCount);
    for (uint32_t i = 0; i < copyCount; i += 1) {
        IDescriptorSet* srcDescriptorSet = copies[i].srcDescriptorSet;
        if (srcDescriptorSet->handleType() != ObjectType::DescriptorSet) {
            /* TODO: error */
            return;
        }

        /* TODO: validation */

        vkCopies[i].sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
        vkCopies[i].srcSet = srcDescriptorSet->handle<VkDescriptorSet>();
        vkCopies[i].srcBinding = copies[i].srcBinding;
        vkCopies[i].srcArrayElement = copies[i].srcElement;
        vkCopies[i].dstSet = _setData.vkDescriptorSet;
        vkCopies[i].dstBinding = copies[i].dstBinding;
        vkCopies[i].dstArrayElement = copies[i].dstElement;
        vkCopies[i].descriptorCount = copies[i].count;
    }

    _setData.poolData.deviceData.functionPointers.device10.vkUpdateDescriptorSets(_setData.poolData.deviceData.vkDevice, 0, nullptr, static_cast<uint32_t>(vkCopies.size()), vkCopies.empty() ? nullptr : &vkCopies[0]);
}

/* IDescriptorSetLayoutInfoQuery */
uint32_t VulkanDescriptorSet::queryDescriptorBindingCount() const noexcept {
    return _layoutInfo.info.bindingCount;
}

uint32_t VulkanDescriptorSet::queryDescriptorBindingIDByIndex(uint32_t index) const noexcept {
    if (index > _layoutInfo.info.bindingCount) {
        return std::numeric_limits<uint32_t>::max();
    }

    return _layoutInfo.info.bindings[index].binding;
}

bool VulkanDescriptorSet::queryDescriptorBindingInfo(uint32_t binding, DescriptorBindingInfo* info) const noexcept {
    for (uint32_t i = 0; i < _layoutInfo.info.bindingCount; i += 1) {
        if (_layoutInfo.info.bindings[i].binding == binding) {
            *info = _layoutInfo.info.bindings[i];
            return true;
        }
    }

    return false;
}

uint32_t VulkanDescriptorSet::queryDescriptorTypeCount(DescriptorFlags flags) const noexcept {
    for (uint32_t i = 0; i < _layoutInfo.info.bindingCount; i += 1) {
        if (_layoutInfo.info.bindings[i].flags == flags) {
            return _layoutInfo.info.bindings[i].count;
        }
    }

    return 0;
}

/* IHandled */
uint64_t VulkanDescriptorSet::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_setData.vkDescriptorSet);
}

ObjectType VulkanDescriptorSet::handleType() const noexcept {
    return ObjectType::DescriptorSet;
}

void const* VulkanDescriptorSet::vkData() const noexcept {
    return &_setData;
}

/* IChild */
IParent* VulkanDescriptorSet::parent() const noexcept {
    return _device->queryInterface<IParent>();
}

/* IInterface */
void* VulkanDescriptorSet::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IDescriptorSetLayoutInfoQuery::iid()) {
        return static_cast<IDescriptorSetLayoutInfoQuery*>(this);
    } else if (iid == IDescriptorSet::iid()) {
        return static_cast<IDescriptorSet*>(this);
    }

    return nullptr;
}

VulkanDescriptorPool::VulkanDescriptorPool(bool inheritedHandle, IDevice* device, DescriptorPoolInfo const& info, VulkanDescriptorPoolData const& poolData) : _inheritedHandle(inheritedHandle), _device(device), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _poolData(poolData) {
    _device->retain();

    _device->label(this, fmt::label(_device, this, "{} max sets, ({})", queryDescriptorSetCapacity(), flags()).c_str());
}

VulkanDescriptorPool::~VulkanDescriptorPool() {
    ParentByVector::disownAll();
    _device->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        _poolData.deviceData.functionPointers.device10.vkDestroyDescriptorPool(_poolData.deviceData.vkDevice, _poolData.vkDescriptorPool, _poolData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    _device->release();
}

/* IDescriptorPool */
DescriptorPoolFlags VulkanDescriptorPool::flags() const noexcept {
    return _info.flags;
}

uint32_t VulkanDescriptorPool::queryDescriptorSetCapacity() const noexcept {
    return _info.maxDescriptorSets;
}

uint32_t VulkanDescriptorPool::queryDescriptorSetAllocatedCount() const noexcept {
    uint32_t count = 0;
    for (uint32_t i = 0; true; i += 1) {
        IDescriptorSet* set = IParent::enumerateChildren<IDescriptorSet>(i);
        if (set == nullptr) {
            break;
        }

        count += 1;
    }

    return count;
}

uint32_t VulkanDescriptorPool::queryDescriptorSetAvailability() const noexcept {
    return queryDescriptorSetAllocatedCount() - queryDescriptorSetAllocatedCount();
}

uint32_t VulkanDescriptorPool::queryDescriptorTypeCapacity(DescriptorFlags flags) const noexcept {
    auto it = _info.descriptors.find(flags);
    if (it == _info.descriptors.end()) {
        return 0;
    }

    return it->second;
}

uint32_t VulkanDescriptorPool::queryDescriptorTypeAllocatedCount(DescriptorFlags flags) const noexcept {
    uint32_t count = 0;
    for (uint32_t i = 0; true; i += 1) {
        IDescriptorSet* set = IParent::enumerateChildren<IDescriptorSet>(i);
        if (set == nullptr) {
            break;
        }

        count += set->queryDescriptorTypeCount(flags);
    }

    return count;
}

uint32_t VulkanDescriptorPool::queryDescriptorTypeAvailability(DescriptorFlags flags) const noexcept {
    return queryDescriptorTypeCapacity(flags) - queryDescriptorTypeAllocatedCount(flags);
}

Result VulkanDescriptorPool::allocateDescriptorSets(IDescriptorSetLayout* layout, uint32_t descriptorSetCount, IDescriptorSet** descriptorSets) noexcept {
    if (layout->handleType() != ObjectType::DescriptorSetLayout) {
        /* TODO: error */
        return Result::ErrorUnknown;
    }

    VkDescriptorSetLayout vkDescriptorSetLayout = layout->handle<VkDescriptorSetLayout>();
    std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts(descriptorSetCount, vkDescriptorSetLayout);

    VkDescriptorSetAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = _poolData.vkDescriptorPool;
    allocateInfo.descriptorSetCount = descriptorSetCount;
    allocateInfo.pSetLayouts = (vkDescriptorSetLayouts.empty() ? nullptr : &vkDescriptorSetLayouts[0]);

    std::vector<VkDescriptorSet> vkDescriptorSets(descriptorSetCount);
    Result result = castEnum<Result>(_poolData.deviceData.functionPointers.device10.vkAllocateDescriptorSets(_poolData.deviceData.vkDevice, &allocateInfo, &vkDescriptorSets[0]));
    if (result != Result::Success) {
        return result;
    }

    std::vector<DescriptorBindingInfo> layoutBindings(layout->queryDescriptorBindingCount());
    for (uint32_t i = 0; i < layout->queryDescriptorBindingCount(); i += 1) {
        uint32_t binding = layout->queryDescriptorBindingIDByIndex(i);
        layout->queryDescriptorBindingInfo(binding, &layoutBindings[i]);
    }

    DescriptorSetLayoutInfo layoutInfo = {};
    layoutInfo.flags = layout->flags();
    layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
    layoutInfo.bindings = (layoutBindings.empty() ? nullptr : &layoutBindings[0]);

    for (uint32_t i = 0; i < descriptorSetCount; i += 1) {
        VulkanDescriptorSetData setData = VulkanDescriptorSetData(_poolData, vkDescriptorSets[i]);

        try {
            descriptorSets[i] = new VulkanDescriptorSet(false, this, layoutInfo, setData);
        } catch (std::runtime_error err) {
            /* TODO: handle error */
            return Result::ErrorUnknown;
        }
    }

    for (uint32_t i = 0; i < descriptorSetCount; i += 1) {
        adopt(descriptorSets[i]);
    }

    return Result::Success;
}

/* IHandled */
uint64_t VulkanDescriptorPool::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_poolData.vkDescriptorPool);
}

ObjectType VulkanDescriptorPool::handleType() const noexcept {
    return ObjectType::DescriptorPool;
}

void const* VulkanDescriptorPool::vkData() const noexcept {
    return &_poolData;
}

/* IChild */
IParent* VulkanDescriptorPool::parent() const noexcept {
    return _device->queryInterface<IParent>();
}

/* IInterface */
void* VulkanDescriptorPool::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IDescriptorPool::iid()) {
        return static_cast<IDescriptorPool*>(this);
    }

    return nullptr;
}

}

}
