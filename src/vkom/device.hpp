#pragma once

#include <limits>

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

inline const uint32_t QUEUE_FAMILY_ANY = std::numeric_limits<uint32_t>::max();

class IQueue;
class IHeap;

class IDevice : virtual public IHandled, virtual public ICollected, virtual public IParent, virtual public IChild, virtual public IDispatchable {
public:
    virtual Result waitIdle() const noexcept = 0;
    virtual bool queryExtension(const char* extension) const noexcept = 0;

    virtual Result acquireQueue(uint32_t family, QueueFlags flags, IQueue** queue) noexcept = 0;

    virtual IHeap* defaultHeap() noexcept = 0;
    virtual Result createHeap(BufferUsageFlags bufferUsages, TextureUsageFlags textureUsages, MemoryLocationFlags memoryLocation, IHeap** heap) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("8af16862-cec4-4dbc-9fe7-c00a6eb9e41c");
        return iid;
    }
};

}
