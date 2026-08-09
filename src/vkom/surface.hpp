#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

struct SurfaceWSIInfo {
    SurfaceWSIType type;
    uint64_t windowHandle;
    uint64_t displayHandle;
    uint64_t miscHandle;
};

class ISurface : virtual public IHandled, virtual public ICollected, virtual public IChild {
public:
    virtual void getInfo(SurfaceWSIInfo* info) const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("c46f18b6-0973-4a58-9442-96bc768014a4");
        return iid;
    }
};

}
