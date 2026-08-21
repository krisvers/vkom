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

class IBufferView : virtual public IResourceView {
public:
    virtual void getInfo(BufferViewInfo* info) const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("a13e4845-9e7e-4872-b96e-4159372e2a03");
        return iid;
    }
};

class IBuffer : virtual public IResource, virtual public IParent {
public:
    virtual void getInfo(BufferInfo* info) const noexcept = 0;

    virtual Result createView(BufferViewInfo const* info, IBufferView** view) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("14c8cbe4-33c5-4240-b34c-6cde96102b43");
        return iid;
    }
};

class ITransferSourceBuffer : virtual public IBuffer {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("b0fc9658-894e-4b8c-8c25-e515c712815d");
        return iid;
    }
};

class ITransferDestinationBuffer : virtual public IBuffer {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("f3c33c1b-ae91-4c0e-ba48-f1beb0388408");
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
        static IID iid = IID("dce2ed25-e2fc-4d5b-854c-6baa6132bffb");
        return iid;
    }
};

class IUniformBuffer : virtual public IBuffer {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("05b328fc-0d25-47e2-8efd-989130740eb7");
        return iid;
    }
};

class IStorageBuffer : virtual public IBuffer {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("50895ae3-b578-456a-8e13-e1fa8205305b");
        return iid;
    }
};

class IUniformTexelBuffer : virtual public IBuffer {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("6533f3aa-c35b-4486-ac5d-555f2e8e3712");
        return iid;
    }
};

class IStorageTexelBuffer : virtual public IBuffer {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("2da4713d-7687-4435-960b-7d839eed61fb");
        return iid;
    }
};

class IDeviceAddressBuffer : virtual public IBuffer {
public:
    virtual uint64_t deviceAddress() const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("d71bdda8-b94a-4d99-b4ce-9d8dfbe90cf8");
        return iid;
    }
};

}
