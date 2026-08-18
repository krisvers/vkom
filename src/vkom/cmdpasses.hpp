#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

struct ComputePassDescriptor {
    /* TODO: */
};

struct RenderPassDescriptor {
    /* TODO: description */
};

class IPipeline;
class IPipelineLayout;

class IPass : virtual public IChild {
public:
    virtual void bindPipeline(IPipeline* pipeline) noexcept = 0;
    virtual void pushConstants(IPipelineLayout* layout, ShaderStageFlags stages, uint32_t offset, uint32_t size, void const* data) noexcept = 0;
    virtual void end() noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("dc292f09-0fb4-467c-908f-94f41b2b3819");
        return iid;
    }
};

class IComputePass : virtual public IPass {
public:
    virtual void dispatch(uint32_t width, uint32_t height, uint32_t depth) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("c5062949-36c0-470b-bc5f-561caf85cae2");
        return iid;
    }
};

class IRenderPass : public IPass {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("87e5dc28-00b3-40d6-a174-8192e7809b68");
        return iid;
    }
};

}
