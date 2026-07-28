#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

struct CommandBatchSubmitInfo {
    /* TODO: parameters */
};

class ICommandEncoder;

class ICommandBatch : public INullable, public IHandled, public IParent, public IChild, public IDispatchable {
public:
    virtual Result record(ICommandEncoder* encoder) noexcept = 0;
    virtual Result submit(CommandBatchSubmitInfo const* submitInfo) noexcept = 0;
    virtual void discard() noexcept = 0;

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

}