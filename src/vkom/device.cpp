#include <vkom/device.hpp>

namespace vkom {

bool IDevice::supportsInterface(IID const& iid) const noexcept {
    return (iid == IDEVICE_IID) || INullable::supportsInterface(iid) || IHandled::supportsInterface(iid) || ICollected::supportsInterface(iid) || IParent::supportsInterface(iid) || IChild::supportsInterface(iid) || IDispatchable::supportsInterface(iid) ;
}

}
