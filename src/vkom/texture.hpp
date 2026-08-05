#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>
#include <vkom/resource.hpp>

namespace vkom {

struct TextureDimensions {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t layers;
    uint32_t mips;
};

struct TexturePosition {
    uint32_t x;
    uint32_t y;
    uint32_t z;
    uint32_t layer;
    uint32_t mip;
};

struct TextureInfo {
    TextureDimensions dimensions;
    Format format;
    TextureUsageFlags usage;
    uint32_t samples;
    MemoryLocationFlags location;
    bool linearTiling;
    bool queueConcurrency;
};

class ITexture : virtual public IResource {
public:
    virtual void getInfo(TextureInfo* info) const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("1f198966-b5ae-402d-880a-ce0e5e4cb79e");
        return iid;
    }
};

}
