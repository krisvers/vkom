#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

struct ResourceAllocationInfo {
    uint64_t resourceSize;
    uint64_t allocationLocalOffset;
    uint64_t allocationLocalSize;
};

class IResource : virtual public IHandled, virtual public ICollected, virtual public IChild {
public:
    virtual void getAllocationInfo(ResourceAllocationInfo* info) const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("af77869c-6bd9-4f4f-9d31-87f993bf1ba6");
        return iid;
    }
};

}
