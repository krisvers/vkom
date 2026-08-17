#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

#include <vkom/resource.hpp>
#include <vkom/buffer.hpp>
#include <vkom/texture.hpp>

namespace vkom {

struct ResourceAliasingInfo {
    IResource* resource;
    uint64_t localOffset;
};

class IHeap : virtual public IHandled, virtual public ICollected, virtual public IParent, virtual public IChild {
public:
    virtual Result createBuffer(BufferInfo const* info, IBuffer** buffer) noexcept = 0;
    virtual Result createAliasedBuffer(BufferInfo const* info, ResourceAliasingInfo const* aliasingInfo, IBuffer** buffer) noexcept = 0;

    virtual Result createTexture(TextureInfo const* info, ITexture** texture) noexcept = 0;
    virtual Result createAliasedTexture(TextureInfo const* info, ResourceAliasingInfo const* aliasingInfo, ITexture** texture) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("d9803cce-d32b-406d-9e59-880d6bfcc314");
        return iid;
    }
};

}
