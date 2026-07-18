#pragma once

#include <vkom/interface.hpp>

namespace vkom {

inline const IID IDYNLIB_IID = IID("b5130b86-58bb-4868-b040-8aed3202cfc8");

class IDynlib : virtual public IInterface {
public:
    virtual bool valid() const noexcept = 0;
    virtual void* loadSymbol(const char* symbol) = 0;
    virtual void destroy() = 0;

    template<typename PFN>
    PFN loadSymbol(const char* symbol) {
        return reinterpret_cast<PFN>(loadSymbol(symbol));
    }

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

IDynlib* loadDynlib(const char* path);

}
