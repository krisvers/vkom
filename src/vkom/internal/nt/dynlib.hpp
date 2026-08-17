#pragma once

#include <vkom/platform.hpp>
#include <vkom/dynlib.hpp>

#ifdef VKOM_PLATFORM_FAMILY_NT

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace vkom {

namespace internal {

namespace nt {

class NTDynlib final : public IDynlib {
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
    void* queryInterface(IID const& iid) noexcept override;
};

}

}

}

#endif
