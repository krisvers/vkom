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

class ITextureView : virtual public IResourceView {
public:
    virtual void getInfo(TextureViewInfo* info) const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("b3aa91cb-8564-4117-a67a-5853ddfd7260");
        return iid;
    }
};

class ITexture : virtual public IResource, virtual public IParent {
public:
    virtual void getInfo(TextureInfo* info) const noexcept = 0;

    virtual Result createView(TextureViewInfo const* info, ITextureView** view) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("1f198966-b5ae-402d-880a-ce0e5e4cb79e");
        return iid;
    }
};

class ITransferSourceTexture : virtual public ITexture {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("115b7edf-43c9-4777-a630-a860ab71fbdd");
        return iid;
    }
};

class ITransferDestinationTexture : virtual public ITexture {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("d28ac197-2f52-43c6-b3fa-47765dc57143");
        return iid;
    }
};

class ISampledTexture : virtual public ITexture {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("5ac54664-70f1-4a79-891b-b0c7fc3bd5c7");
        return iid;
    }
};

class IStorageTexture : virtual public ITexture {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("f578bb81-7304-4cad-8a43-3401eba128a7");
        return iid;
    }
};

class IRenderTarget : virtual public ITexture {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("7bb6dc84-5f9a-4e43-afe3-06fd3de42f54");
        return iid;
    }
};

class IDepthStencilTarget : virtual public ITexture {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("1f41f18c-cdbb-4b6e-8a23-e79a4b3e8b07");
        return iid;
    }
};

class ITransientTarget : virtual public ITexture {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("af76676b-eba8-43af-914b-8d08a7af9f87");
        return iid;
    }
};

class IInputTarget : virtual public ITexture {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("af76676b-eba8-43af-914b-8d08a7af9f87");
        return iid;
    }
};

}
