#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

inline const IID IQUEUE_IID = IID("0a67cb8d-be6d-4365-b210-1847a84075d9");

class IQueue : public INullable, public IHandled, public ICollected, public IParent, public IChild, public IDispatchable {
public:
    virtual uint32_t family() const noexcept = 0;
    virtual uint32_t index() const noexcept = 0;
    virtual QueueFlags flags() const noexcept = 0;

    virtual Result waitIdle() const noexcept = 0;

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

}