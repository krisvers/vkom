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

};

class IBufferView;

class IBuffer : public IResource {
public:
    virtual void getInfo(BufferInfo* info) const noexcept = 0;
    virtual uint64_t deviceAddress() const noexcept = 0;

    virtual Result createView(BufferViewInfo const* info) noexcept = 0;

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

}