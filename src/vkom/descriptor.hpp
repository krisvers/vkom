#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

#include <vkom/resource.hpp>
#include <vkom/sampler.hpp>

namespace vkom {

class IDescriptorSet;

struct DescriptorBindingInfo {
    uint32_t binding;
    DescriptorFlags flags;
    uint32_t count;
    ShaderStageFlags stages;
    ISampler* const* immutableSamplers;
};

struct DescriptorSetLayoutInfo {
    DescriptorSetLayoutFlags flags;
    uint32_t bindingCount;
    DescriptorBindingInfo const* bindings;
};

struct DescriptorTextureInfo {
    ISampler* sampler;
    TextureLayout layout;
};

struct DescriptorWrite {
    uint32_t binding;
    uint32_t element;
    uint32_t count;
    IResourceView* const* views;
    DescriptorTextureInfo const* textureInfos;
};

struct DescriptorCopy {
    IDescriptorSet* srcDescriptorSet;
    uint32_t srcBinding;
    uint32_t srcElement;
    uint32_t dstBinding;
    uint32_t dstElement;
    uint32_t count;
};

struct DescriptorPoolDescriptorInfo {
    DescriptorFlags flags;
    uint32_t count;
};

struct DescriptorPoolInfo {
    DescriptorPoolFlags flags;
    uint32_t maxDescriptorSets;
    uint32_t descriptorCount;
    DescriptorPoolDescriptorInfo const* descriptors;
};

class IDescriptorSetLayoutInfoQuery : virtual public IBase {
public:
    virtual uint32_t queryDescriptorBindingCount() const noexcept = 0;
    virtual uint32_t queryDescriptorBindingIDByIndex(uint32_t index) const noexcept = 0;
    virtual bool queryDescriptorBindingInfo(uint32_t binding, DescriptorBindingInfo* info) const noexcept = 0;
    virtual uint32_t queryDescriptorTypeCount(DescriptorFlags flags) const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("54972387-a3e9-4d43-95fe-d7dcc02ee4de");
        return iid;
    }
};

class IDescriptorSetLayout : virtual public IDescriptorSetLayoutInfoQuery, virtual public IHandled, virtual public ICollected, virtual public IChild {
public:
    virtual DescriptorSetLayoutFlags flags() const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("917e7a5d-b82f-48c9-b619-83c11b2d73ba");
        return iid;
    }
};

class IDescriptorSet : virtual public IDescriptorSetLayoutInfoQuery, virtual public IHandled, virtual public ICollected, virtual public IChild {
public:
    virtual DescriptorPoolFlags poolFlags() const noexcept = 0;

    virtual void write(uint32_t writeCount, DescriptorWrite const* writes) noexcept = 0;
    virtual void copy(uint32_t copyCount, DescriptorCopy const* copies) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("3f76391e-389e-42e6-886e-a364af09944e");
        return iid;
    }
};

class IDescriptorPool : virtual public IHandled, virtual public ICollected, virtual public IParent, virtual public IChild {
public:
    virtual DescriptorPoolFlags flags() const noexcept = 0;

    virtual uint32_t queryDescriptorSetCapacity() const noexcept = 0;
    virtual uint32_t queryDescriptorSetAllocatedCount() const noexcept = 0;
    virtual uint32_t queryDescriptorSetAvailability() const noexcept = 0;
    virtual uint32_t queryDescriptorTypeCapacity(DescriptorFlags flags) const noexcept = 0;
    virtual uint32_t queryDescriptorTypeAllocatedCount(DescriptorFlags flags) const noexcept = 0;
    virtual uint32_t queryDescriptorTypeAvailability(DescriptorFlags flags) const noexcept = 0;

    virtual Result allocateDescriptorSets(IDescriptorSetLayout* layout, uint32_t descriptorSetCount, IDescriptorSet** descriptorSets) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("00c75e0f-319e-44ad-abac-6ae51abe4b05");
        return iid;
    }
};

}
