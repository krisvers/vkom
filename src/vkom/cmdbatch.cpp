#include <vkom/cmdbatch.hpp>

namespace vkom {

bool ICommandBatch::supportsInterface(IID const& iid) const noexcept {
    return (iid == ICOMMANDBATCH_IID) || INullable::supportsInterface(iid) || IHandled::supportsInterface(iid) || IChild::supportsInterface(iid) || IDispatchable::supportsInterface(iid);
}

}
