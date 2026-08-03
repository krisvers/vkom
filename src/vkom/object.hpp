#pragma once

#include <cstdint>

#include <vkom/interface.hpp>
#include <vkom/enums.hpp>

namespace vkom {

class IBase : public IInterface {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("f332f959-db11-4a85-946a-a6aec73c2135");
        return iid;
    }
};

class INullable : public IBase {
public:
    virtual bool isNull() const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("6536e860-8efc-4c36-b54a-2fd39f70bf38");
        return iid;
    }
};

class IHandled : public IBase {
public:
    virtual uint64_t handle() const noexcept = 0;
    virtual ObjectType handleType() const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("96c21c2f-6f30-4ab7-9dd9-e3851c99ff7c");
        return iid;
    }
};

class ICollected : public IBase {
public:
    virtual uint32_t release() = 0;
    virtual uint32_t retain() = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("6c40f1c3-7e70-40c1-ba78-d69492aff2e0");
        return iid;
    }
};

class IChild;

class IParent : public IBase {
public:
    virtual bool hasChild(IChild const* child) const noexcept = 0;
    virtual IChild* enumerateChildren(uint32_t id) const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("705991be-d0d0-4269-96f2-2efd2b0f5647");
        return iid;
    }
};

class IChild : public IBase {
public:
    virtual IParent* parent() const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("a89c1f03-8a9f-403e-9db3-0d706ea61cb5");
        return iid;
    }
};

class IDispatchable : public IBase {
public:
    virtual void* loadDispatchSymbol(const char* symbol) = 0;

    template<typename PFN>
    PFN loadDispatchSymbol(const char* symbol) {
        return reinterpret_cast<PFN>(loadDispatchSymbol(symbol));
    }

    static inline IID const& iid() noexcept {
        static IID iid = IID("b0055161-0823-4760-9352-108c865c7648");
        return iid;
    }
};

}
