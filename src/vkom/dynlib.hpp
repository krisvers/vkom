#pragma once

#include <vkom/interface.hpp>

namespace vkom {

class IDynlib : public IInterface {
public:
    virtual bool valid() const noexcept = 0;
    virtual void* loadSymbol(const char* symbol) = 0;
    virtual void destroy() = 0;

    template<typename PFN>
    PFN loadSymbol(const char* symbol) {
        return reinterpret_cast<PFN>(loadSymbol(symbol));
    }

    static inline IID const& iid() noexcept {
        static IID iid = IID("b5130b86-58bb-4868-b040-8aed3202cfc8");
        return iid;
    }
};

IDynlib* loadDynlib(const char* path);

}
