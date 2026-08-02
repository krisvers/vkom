#pragma once

#include <vkom/enums.hpp>

namespace vkom {

struct ComputePassDescriptor {
    /* TODO: description */
};

struct RenderPassDescriptor {
    /* TODO: description */
};

inline const IID IPASS_IID = IID("dc292f09-0fb4-467c-908f-94f41b2b3819");

class IPass : public IChild {
public:
    virtual void end() noexcept = 0;

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

inline const IID ICOMPUTEPASS_IID = IID("c5062949-36c0-470b-bc5f-561caf85cae2");

class IComputePipeline;

class IComputePass : public IPass {
public:
    virtual void bindPipeline(IComputePipeline* pipeline) noexcept = 0;

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

inline const IID IRENDERPASS_IID = IID("87e5dc28-00b3-40d6-a174-8192e7809b68");

class IGraphicsPipeline;

class IRenderPass : public IPass {
public:

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

}
