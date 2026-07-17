#include <vkom/instance.hpp>

namespace vkom {

bool IInstance::supportsInterface(IID const& iid) const noexcept {
    return (iid == IINSTANCE_IID) || INullable::supportsInterface(iid) || IHandled::supportsInterface(iid) || ICollected::supportsInterface(iid) || IParent::supportsInterface(iid) || IDispatchable::supportsInterface(iid);
}

}
