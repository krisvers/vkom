#include <vkom/dynlib.hpp>

namespace vkom {

bool IDynlib::supportsInterface(IID const& iid) const noexcept {
    return (iid == IDYNLIB_IID);
}

}
