#include <vkom/internal/nt/dynlib.hpp>

#ifdef VKOM_PLATFORM_FAMILY_NT

#include <stdexcept>

namespace vkom {

namespace internal {

namespace nt {

NTDynlib::NTDynlib(HMODULE libraryHandle) : _libraryHandle(libraryHandle) {}

NTDynlib::~NTDynlib() {
    if (_libraryHandle != nullptr) {
        FreeLibrary(_libraryHandle);
    }
}

bool NTDynlib::valid() const noexcept {
    return (_libraryHandle != nullptr);
}

void* NTDynlib::loadSymbol(const char* symbol) {
    if (_libraryHandle == nullptr) {
        throw std::runtime_error("Can't load symbol from Dynlib: handle is invalid");
    }

    return reinterpret_cast<void*>(GetProcAddress(_libraryHandle, symbol));
}

void NTDynlib::destroy() {
    delete this;
}

bool NTDynlib::supportsInterface(IID const& iid) const noexcept {
    return IDynlib::supportsInterface(iid);
}

}

}

IDynlib* loadDynlib(const char* path) {
    HMODULE libraryHandle = LoadLibraryA(path);
    if (libraryHandle == nullptr) {
        return nullptr;
    }

    return new internal::nt::NTDynlib(libraryHandle);
}

}

#endif
