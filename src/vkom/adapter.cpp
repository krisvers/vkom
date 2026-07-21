#include <vkom/adapter.hpp>

namespace vkom {

bool IAdapter::supportsInterface(IID const& iid) const noexcept {
    return (iid == IADAPTER_IID) || IHandled::supportsInterface(iid) || IChild::supportsInterface(iid) || IParent::supportsInterface(iid);
}

}
