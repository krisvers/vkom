#pragma once

#include <vkom/enums.hpp>

namespace vkom {

struct ComputePassDescriptor {
    /* TODO: description */
};

struct RenderPassDescriptor {
    /* TODO: description */
};

class IPass : public IChild {
public:
    virtual void end() noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("dc292f09-0fb4-467c-908f-94f41b2b3819");
        return iid;
    }
};

class IComputePipeline;

class IComputePass : public IPass {
public:
    virtual void bindPipeline(IComputePipeline* pipeline) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("c5062949-36c0-470b-bc5f-561caf85cae2");
        return iid;
    }
};

class IGraphicsPipeline;

class IRenderPass : public IPass {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("87e5dc28-00b3-40d6-a174-8192e7809b68");
        return iid;
    }
};

}
