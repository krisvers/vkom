#include <vkom/resource.hpp>

namespace vkom {

bool IResource::supportsInterface(IID const& iid) const noexcept {
    return (iid == IRESOURCE_IID) || INullable::supportsInterface(iid) || IHandled::supportsInterface(iid) || ICollected::supportsInterface(iid) || IParent::supportsInterface(iid) || IChild::supportsInterface(iid);
}

}
