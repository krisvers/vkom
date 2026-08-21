#pragma once

#include <vkom/descriptor.hpp>

#include <vector>
#include <unordered_map>

#include <vkom/enums.hpp>
#include <vkom/device.hpp>
#include <vkom/adapter.hpp>
#include <vkom/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

struct VulkanDescriptorSetLayoutInfo {
    DescriptorSetLayoutInfo info;
    std::vector<DescriptorBindingInfo> bindings;
    std::vector<ISampler*> bindingImmutableSamplers;
};

struct VulkanDescriptorPoolInfo {
    DescriptorPoolFlags flags;
    uint32_t maxDescriptorSets;
    std::unordered_map<DescriptorFlags, uint32_t> descriptors;
};

class VulkanDescriptorSetLayout final : virtual public IDescriptorSetLayout, virtual public CollectedByHeap {
private:
    bool _inheritedHandle = false;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanDescriptorSetLayoutInfo _info;
    VulkanDescriptorSetLayoutData _layoutData;

public:
    VulkanDescriptorSetLayout(bool inheritedHandle, IDevice* device, DescriptorSetLayoutInfo const& info, VulkanDescriptorSetLayoutData const& layoutData);
    ~VulkanDescriptorSetLayout();

    /* IDescriptorSetLayout */
    DescriptorSetLayoutFlags flags() const noexcept override;

    /* IDescriptorSetLayoutInfoQuery */
    uint32_t queryDescriptorBindingCount() const noexcept override;
    uint32_t queryDescriptorBindingIDByIndex(uint32_t index) const noexcept override;
    bool queryDescriptorBindingInfo(uint32_t binding, DescriptorBindingInfo* info) const noexcept override;
    uint32_t queryDescriptorTypeCount(DescriptorFlags flags) const noexcept override;

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    void const* vkData() const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

class VulkanDescriptorSet final : virtual public IDescriptorSet, virtual public CollectedByHeap {
private:
    bool _inheritedHandle = false;
    IDescriptorPool* _pool = nullptr;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanDescriptorSetLayoutInfo _layoutInfo;
    VulkanDescriptorSetData _setData;

public:
    VulkanDescriptorSet(bool inheritedHandle, IDescriptorPool* pool, DescriptorSetLayoutInfo const& info, VulkanDescriptorSetData const& setData);
    ~VulkanDescriptorSet();

    /* IDescriptorSet */
    DescriptorPoolFlags poolFlags() const noexcept override;

    void write(uint32_t writeCount, DescriptorWrite const* writes) noexcept override;
    void copy(uint32_t copyCount, DescriptorCopy const* copies) noexcept override;

    /* IDescriptorSetLayoutInfoQuery */
    uint32_t queryDescriptorBindingCount() const noexcept override;
    uint32_t queryDescriptorBindingIDByIndex(uint32_t index) const noexcept override;
    bool queryDescriptorBindingInfo(uint32_t binding, DescriptorBindingInfo* info) const noexcept override;
    uint32_t queryDescriptorTypeCount(DescriptorFlags flags) const noexcept override;

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    void const* vkData() const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

class VulkanDescriptorPool final : virtual public IDescriptorPool, virtual public CollectedByHeap, virtual public ParentByVector {
private:
    bool _inheritedHandle = false;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanDescriptorPoolInfo _info;
    VulkanDescriptorPoolData _poolData;

public:
    VulkanDescriptorPool(bool inheritedHandle, IDevice* device, DescriptorPoolInfo const& info, VulkanDescriptorPoolData const& poolData);
    ~VulkanDescriptorPool();

    /* IDescriptorPool */
    DescriptorPoolFlags flags() const noexcept override;

    uint32_t queryDescriptorSetCapacity() const noexcept override;
    uint32_t queryDescriptorSetAllocatedCount() const noexcept override;
    uint32_t queryDescriptorSetAvailability() const noexcept override;
    uint32_t queryDescriptorTypeCapacity(DescriptorFlags flags) const noexcept override;
    uint32_t queryDescriptorTypeAllocatedCount(DescriptorFlags flags) const noexcept override;
    uint32_t queryDescriptorTypeAvailability(DescriptorFlags flags) const noexcept override;

    Result allocateDescriptorSets(IDescriptorSetLayout* layout, uint32_t descriptorSetCount, IDescriptorSet** descriptorSets) noexcept override;

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    void const* vkData() const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

}

}
