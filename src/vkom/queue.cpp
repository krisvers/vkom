#include <vkom/queue.hpp>

namespace vkom {

bool IQueue::supportsInterface(IID const& iid) const noexcept {
    return (iid == IQUEUE_IID) || INullable::supportsInterface(iid) || IHandled::supportsInterface(iid) || ICollected::supportsInterface(iid) || IParent::supportsInterface(iid) || IChild::supportsInterface(iid) || IDispatchable::supportsInterface(iid) ;
}

}
