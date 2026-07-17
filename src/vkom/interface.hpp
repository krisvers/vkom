#pragma once

#include <vkom/uuid.hpp>

namespace vkom {

using IID = UUID;

class IInterface {
public:
    virtual bool supportsInterface(IID const& iid) const noexcept = 0;
};

}
