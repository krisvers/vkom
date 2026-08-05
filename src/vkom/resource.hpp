#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

class IHeap;

class IResource : virtual public IHandled, virtual public ICollected, virtual public IParent, virtual public IChild {
public:
    virtual uint64_t resourceSize() const noexcept = 0;
    virtual uint64_t allocationOffset() const noexcept = 0;
    virtual uint64_t allocationSize() const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("af77869c-6bd9-4f4f-9d31-87f993bf1ba6");
        return iid;
    }
};

}
