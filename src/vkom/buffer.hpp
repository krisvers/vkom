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

    virtual Result createView(BufferViewInfo const* info, IBufferView** view) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("14c8cbe4-33c5-4240-b34c-6cde96102b43");
        return iid;
    }
};

class IIndirectBuffer : virtual public IBuffer {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("587117cd-238d-4331-8b93-c56fa6d9ac89");
        return iid;
    }
};

class IIndexBuffer : virtual public IBuffer {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("9f4b5533-46b7-4788-8cfc-c9f99198ec74");
        return iid;
    }
};

class IVertexBuffer : virtual public IBuffer {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("5930a704-ce7d-4107-b076-af1907d2aa9f");
        return iid;
    }
};

class IUniformBuffer : virtual public IBuffer {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("5930a704-ce7d-4107-b076-af1907d2aa9f");
        return iid;
    }
};

class IStorageBuffer : virtual public IBuffer {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("5930a704-ce7d-4107-b076-af1907d2aa9f");
        return iid;
    }
};

class ITexelBuffer : virtual public IBuffer {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("5930a704-ce7d-4107-b076-af1907d2aa9f");
        return iid;
    }
};

class IDeviceAddressBuffer : virtual public IBuffer {
public:
    virtual uint64_t deviceAddress() const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("5930a704-ce7d-4107-b076-af1907d2aa9f");
        return iid;
    }
};

}
