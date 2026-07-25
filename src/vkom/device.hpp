#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>
#include <vkom/queue.hpp>

namespace vkom {

inline const IID IDEVICE_IID = IID("8af16862-cec4-4dbc-9fe7-c00a6eb9e41c");

class IDevice : public INullable, public IHandled, public ICollected, public IParent, public IChild, public IDispatchable {
public:
    virtual Result waitIdle() const noexcept = 0;
    virtual Result acquireQueue(QueueFlags flags, IQueue** queue);

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

}