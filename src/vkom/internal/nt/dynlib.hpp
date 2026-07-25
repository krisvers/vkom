#pragma once

#include <vkom/platform.hpp>
#include <vkom/dynlib.hpp>

#ifdef VKOM_PLATFORM_FAMILY_NT

#include <windows.h>

namespace vkom {

namespace internal {

namespace nt {

class NTDynlib : public IDynlib {
private:
    HMODULE _libraryHandle = nullptr;

public:
    NTDynlib(HMODULE libraryHandle);
    ~NTDynlib();

    /* IDynlib */
    bool valid() const noexcept override;
    void* loadSymbol(const char* symbol) override;
    void destroy() override;

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

}

}

}

#endif
