#pragma once

#include <limits>

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

inline const IID IDEVICE_IID = IID("8af16862-cec4-4dbc-9fe7-c00a6eb9e41c");
inline const uint32_t QUEUE_FAMILY_ANY = std::numeric_limits<uint32_t>::max();

class IQueue;
class IHeap;

class IDevice : public INullable, public IHandled, public ICollected, public IParent, public IChild, public IDispatchable {
public:
    virtual Result waitIdle() const noexcept = 0;

    virtual Result acquireQueue(uint32_t family, QueueFlags flags, IQueue** queue) noexcept = 0;

    virtual IHeap* defaultHeap() noexcept = 0;
    virtual Result createHeap(BufferUsageFlags bufferUsages, TextureUsageFlags textureUsages, MemoryLocationFlags memoryLocation, IHeap** heap) noexcept = 0;

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

}
