#include <vkom/texture.hpp>

namespace vkom {

bool ITexture::supportsInterface(IID const& iid) const noexcept {
    return (iid == ITEXTURE_IID) || IResource::supportsInterface(iid);
}

}
