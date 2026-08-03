#include <vkom/internal/unix/dynlib.hpp>

#ifdef VKOM_PLATFORM_FAMILY_UNIX

#include <stdexcept>

#include <dlfcn.h>

namespace vkom {

namespace internal {

namespace unix {

UnixDynlib::UnixDynlib(void* libraryHandle) : _libraryHandle(libraryHandle) {}

UnixDynlib::~UnixDynlib() {
    if (_libraryHandle != nullptr) {
        dlclose(_libraryHandle);
    }
}

bool UnixDynlib::valid() const noexcept {
    return (_libraryHandle != nullptr);
}

void* UnixDynlib::loadSymbol(const char* symbol) {
    if (_libraryHandle == nullptr) {
        throw std::runtime_error("Can't load symbol from Dynlib: handle is invalid");
    }

    return reinterpret_cast<void*>(dlsym(_libraryHandle, symbol));
}

void UnixDynlib::destroy() {
    delete this;
}

void* UnixDynlib::queryInterface(IID const& iid) noexcept {
    if (iid == IDynlib::iid()) {
        return static_cast<IDynlib*>(this);
    }

    return nullptr;
}

}

}

IDynlib* loadDynlib(const char* path) {
    void* libraryHandle = dlopen(path, RTLD_LAZY);
    if (libraryHandle == nullptr) {
        return nullptr;
    }

    return new internal::unix::UnixDynlib(libraryHandle);
}

}

#endif
