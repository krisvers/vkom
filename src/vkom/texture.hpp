#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

#include <vkom/resource.hpp>

namespace vkom {

struct TextureExtent {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
};

struct TextureSubresourceDimensions {
    uint32_t layers;
    uint32_t mips;
};

struct TextureDimensions {
    TextureExtent extent;
    TextureSubresourceDimensions subresource;
};

struct TextureXYZ {
    uint32_t x;
    uint32_t y;
    uint32_t z;
};

struct TextureSubresourcePosition {
    uint32_t layer;
    uint32_t mip;
};

struct TexturePosition {
    TextureXYZ xyz;
    TextureSubresourcePosition subresource;
};

struct TextureInfo {
    Format format;
    uint32_t samplesPerTexel;
    TextureUsageFlags usage;
    TextureDimensions dimensions;
    MemoryLocationFlags location;
    bool linearTiling;
    bool queueConcurrency;
};

struct TextureViewInfo {
    Format format;
    TextureViewType type;
    TextureAspectFlags aspectFlags;
    TextureSubresourceDimensions subresourceDimensions;
    TextureSubresourcePosition subresourcePosition;

    TextureChannelSwizzle redSwizzle;
    TextureChannelSwizzle greenSwizzle;
    TextureChannelSwizzle blueSwizzle;
    TextureChannelSwizzle alphaSwizzle;
};

class ITextureView;

class ITexture : virtual public IResource, virtual public IParent {
public:
    virtual void getInfo(TextureInfo* info) const noexcept = 0;

    virtual Result createView(TextureViewInfo const* info, ITextureView** view) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("1f198966-b5ae-402d-880a-ce0e5e4cb79e");
        return iid;
    }
};

}
