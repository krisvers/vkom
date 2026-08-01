#include <vkom/cmdencoder.hpp>

namespace vkom {

bool ICommandEncoder::supportsInterface(IID const& iid) const noexcept {
    return (iid == ICOMMANDENCODER_IID) || INullable::supportsInterface(iid) || IHandled::supportsInterface(iid) || ICollected::supportsInterface(iid) || IParent::supportsInterface(iid) || IChild::supportsInterface(iid) || IDispatchable::supportsInterface(iid);
}

}
