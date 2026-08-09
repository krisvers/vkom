#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

class IFence : virtual public IHandled, virtual public ICollected, virtual public IChild {
public:
    virtual void reset() noexcept = 0;
    virtual bool status() const noexcept = 0;
    virtual Result wait() noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("286a81e9-4b49-4010-ac1f-908407a7d495");
        return iid;
    }
};

}
