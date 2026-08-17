#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

#include <vkom/cmdencoder.hpp>
#include <vkom/cmdbatch.hpp>

namespace vkom {

class IQueue : virtual public IHandled, virtual public ICollected, virtual public IParent, virtual public IChild, virtual public IDispatchable {
public:
    virtual uint32_t family() const noexcept = 0;
    virtual uint32_t index() const noexcept = 0;
    virtual QueueFlags flags() const noexcept = 0;

    virtual Result waitIdle() const noexcept = 0;

    virtual Result acquireCommandEncoder(ICommandEncoder** encoder) noexcept = 0;
    virtual Result acquireCommandBatch(ICommandBatch** batch) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("0a67cb8d-be6d-4365-b210-1847a84075d9");
        return iid;
    }
};

}
