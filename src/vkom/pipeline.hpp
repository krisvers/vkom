#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

class IDescriptorSetLayout;
class IShaderModule;
class IPipeline;
class IPipelineCache;
class IPipelineLayout;

struct ShaderModuleInfo {
    uint64_t length;
    uint32_t const* spirv;
};

struct PushConstantRange {
    ShaderStageFlags stages;
    uint32_t offset;
    uint32_t size;
};

struct PipelineLayoutInfo {
    uint32_t descriptorSetLayoutCount;
    IDescriptorSetLayout* const* descriptorSetLayouts;
    uint32_t pushConstantRangeCount;
    PushConstantRange const* pushConstantRanges;
};

struct SpecializationEntry {
    uint32_t constantID;
    uint32_t offset;
    uint32_t size;
};

struct SpecializationInfo {
    uint32_t entryCount;
    SpecializationEntry const* entries;
    uint32_t size;
    void const* data;
};

struct PipelineShaderInfo {
    PipelineShaderFlags flags;
    IShaderModule* shader;
    ShaderStageFlags stage;
    const char* entry;
    SpecializationInfo specialization;
};

struct PipelineDerivationInfo {
    IPipeline* base;
    int32_t index;
};

struct GraphicsPipelineInfo {
    PipelineFlags flags;
    /* TODO: */
};

struct ComputePipelineInfo {
    PipelineFlags flags;
    PipelineShaderInfo shaderInfo;
    PipelineDerivationInfo derivationInfo;
};

struct RayTracingPipelineInfo {

};

class IShaderModule : virtual public IHandled, virtual public ICollected, virtual public IChild {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("b7b95f71-7855-4fbd-99f1-fbf999775eb7");
        return iid;
    }
};

class IPipelineCache : virtual public IHandled, virtual public ICollected, virtual public IChild {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("9ff6af08-8c28-436a-b30a-e49cea0dc1cb");
        return iid;
    }
};

class IPipelineLayout : virtual public IHandled, virtual public ICollected, virtual public IChild {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("9b1d1832-c600-4786-903e-4d0593b1e29e");
        return iid;
    }
};

class IPipeline : virtual public IHandled, virtual public ICollected, virtual public IChild {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("f9fe7204-1096-4456-acdb-09902f90231f");
        return iid;
    }
};

class IComputePipeline : virtual public IPipeline {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("5fdac3eb-47e5-40e6-9940-65be15d578b4");
        return iid;
    }
};

class IGraphicsPipeline : virtual public IPipeline {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("7efa9b9b-61da-48b0-911d-4c9f2bb38fdd");
        return iid;
    }
};

class IRayTracingPipeline : virtual public IPipeline {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("e5ce59ba-2c36-4a5a-87c6-784490324e8b");
        return iid;
    }
};

}
