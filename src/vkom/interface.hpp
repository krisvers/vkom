#pragma once

#include <vkom/uuid.hpp>

namespace vkom {

using IID = UUID;

class IInterface {
public:
    virtual void* queryInterface(IID const& iid) noexcept = 0;

    template<typename T>
    T* queryInterface() {
        return reinterpret_cast<T*>(queryInterface(T::iid()));
    }
};

}
