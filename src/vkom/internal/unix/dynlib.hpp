#pragma once

#include <vkom/platform.hpp>
#include <vkom/dynlib.hpp>

#ifdef VKOM_PLATFORM_FAMILY_UNIX

namespace vkom {

namespace internal {

namespace unix {

class UnixDynlib final : public IDynlib {
private:
    void* _libraryHandle = nullptr;

public:
    UnixDynlib(void* libraryHandle);
    ~UnixDynlib();

    /* IDynlib */
    bool valid() const noexcept override;
    void* loadSymbol(const char* symbol) override;
    void destroy() override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

IDynlib* loadDynlib(const char* path);

}

}

}

#endif
