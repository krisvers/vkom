#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

class ISemaphore : virtual public IHandled, virtual public ICollected, virtual public IChild {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("286a81e9-4b49-4010-ac1f-908407a7d495");
        return iid;
    }
};

class ITimelineSemaphore : virtual public ISemaphore {
public:
    virtual Result wait(uint64_t value) noexcept = 0;
    virtual uint64_t counter() const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("85fa38ed-f124-4a87-963f-78d8013be721");
        return iid;
    }
};

struct SemaphoreSignalInfo {
    ISemaphore* semaphore;
    uint64_t value;
};

}
