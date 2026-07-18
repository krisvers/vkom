#include <vkom/object.hpp>

namespace vkom {

bool IBase::supportsInterface(IID const& iid) const noexcept {
    return (iid == IBASE_IID);
}

bool INullable::supportsInterface(IID const& iid) const noexcept {
    return (iid == INULLABLE_IID) || IBase::supportsInterface(iid);
}

bool IHandled::supportsInterface(IID const& iid) const noexcept {
    return (iid == IHANDLED_IID) || IBase::supportsInterface(iid);
}

bool ICollected::supportsInterface(IID const& iid) const noexcept {
    return (iid == ICOLLECTED_IID) || IBase::supportsInterface(iid);
}

bool IParent::supportsInterface(IID const& iid) const noexcept {
    return (iid == IPARENT_IID) || IBase::supportsInterface(iid);
}

bool IChild::supportsInterface(IID const& iid) const noexcept {
    return (iid == ICHILD_IID) || IBase::supportsInterface(iid);
}

bool IDispatchable::supportsInterface(IID const& iid) const noexcept {
    return (iid == IDISPATCHABLE_IID) || IBase::supportsInterface(iid);
}

}
