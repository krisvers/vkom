#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

class IHeap;

class IResource : public INullable, public IHandled, public ICollected, public IParent, public IChild {
public:
    virtual uint64_t resourceSize() const noexcept = 0;
    virtual uint64_t allocationOffset() const noexcept = 0;
    virtual uint64_t allocationSize() const noexcept = 0;

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

}