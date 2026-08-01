#include <vkom/buffer.hpp>

namespace vkom {

bool IBuffer::supportsInterface(IID const& iid) const noexcept {
    return (iid == IBUFFER_IID) || IResource::supportsInterface(iid);
}

}
