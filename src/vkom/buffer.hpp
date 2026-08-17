#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

#include <vkom/resource.hpp>

namespace vkom {

struct BufferInfo {
    uint64_t size;
    BufferUsageFlags usage;
    MemoryLocationFlags location;
    bool queueConcurrency;
};

struct BufferViewInfo {
    Format format;
    uint64_t offset;
    uint64_t range;
};

class IBufferView;

class IBuffer : virtual public IResource, virtual public IParent {
public:
    virtual void getInfo(BufferInfo* info) const noexcept = 0;
    virtual uint64_t deviceAddress() const noexcept = 0;

    virtual Result createView(BufferViewInfo const* info, IBufferView** view) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("14c8cbe4-33c5-4240-b34c-6cde96102b43");
        return iid;
    }
};

}
