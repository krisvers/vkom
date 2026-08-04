#pragma once

#include <cstdint>

#include <vkom/interface.hpp>
#include <vkom/enums.hpp>

namespace vkom {

class IBase : virtual public IInterface {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("f332f959-db11-4a85-946a-a6aec73c2135");
        return iid;
    }
};

class IHandled : virtual public IBase {
public:
    virtual uint64_t handle() const noexcept = 0;
    virtual ObjectType handleType() const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("96c21c2f-6f30-4ab7-9dd9-e3851c99ff7c");
        return iid;
    }
};

class ICollected : virtual public IBase {
public:
    virtual uint32_t release() = 0;
    virtual uint32_t retain() = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("6c40f1c3-7e70-40c1-ba78-d69492aff2e0");
        return iid;
    }
};

class IChild;

class IParent : virtual public IBase {
public:
    virtual bool hasChild(IChild const* child) const noexcept = 0;
    virtual IChild* enumerateChildren(uint32_t id, IID const& filter = IID::null()) const noexcept = 0;
    virtual bool adopt(IChild* child) noexcept = 0;
    virtual bool disown(IChild* child) noexcept = 0;

    template<typename T>
    T* enumerateChildren(uint32_t id) const noexcept {
        return enumerateChildren(id, T::iid())->queryInterface<T>();
    }

    static inline IID const& iid() noexcept {
        static IID iid = IID("705991be-d0d0-4269-96f2-2efd2b0f5647");
        return iid;
    }
};

class IChild : virtual public IBase {
public:
    virtual IParent* parent() const noexcept = 0;

    template<typename T>
    T* parent() const noexcept {
        return parent()->queryInterface<T>();
    }

    static inline IID const& iid() noexcept {
        static IID iid = IID("a89c1f03-8a9f-403e-9db3-0d706ea61cb5");
        return iid;
    }
};

class IDispatchable : virtual public IBase {
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
