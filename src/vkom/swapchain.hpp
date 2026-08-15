#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

#include <limits>

#include <vkom/surface.hpp>
#include <vkom/texture.hpp>
#include <vkom/fence.hpp>
#include <vkom/semaphore.hpp>

namespace vkom {

struct SwapchainInfo {
    bool managed;
    uint32_t backbufferCount;
    TextureInfo backbufferInfo;
    SurfaceTransformFlags preTransform;
    CompositeAlphaFlags compositeAlpha;
    uint64_t surfaceFormatBits;
    PresentModeFlags presentModeFlags;
    bool clipped;
};

struct PresentInfo {
    uint32_t waitCount;
    SemaphorePoint const* waits;
    bool requestFence;
};

class IPresentFence : virtual public IFence {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("307ed28c-1ba1-4bb0-b48d-9fd235881a97");
        return iid;
    }
};

class IPresentIDFence : virtual public IPresentFence {
public:
    virtual uint64_t presentID() const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("3d765e58-a614-474f-9eb5-1ed470a37b02");
        return iid;
    }
};

class IBackbuffer : virtual public ITexture {
public:
    virtual uint32_t index() const noexcept = 0;
    virtual Result present(PresentInfo const* info, IPresentFence** fence) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("deeef755-cf7c-48d3-ac67-baaf1345a2d3");
        return iid;
    }
};

class ISwapchain : virtual public IHandled, virtual public ICollected, virtual public IChild, virtual public IParent {
public:
    virtual void getInfo(SwapchainInfo* info) const noexcept = 0;

    virtual ISurface* surface() const noexcept = 0;
    virtual IBackbuffer* enumerateBackbuffers(uint32_t id) const noexcept = 0;

    virtual Result recreate(SwapchainInfo const* info) noexcept = 0;

    virtual Result acquireNextIndex(SemaphorePoint const* signalSemaphore, IFence* signalFence, uint32_t* index, uint64_t timeout = std::numeric_limits<uint64_t>::max()) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("e6ef4a7c-42b0-48f7-95f7-2dbaac560886");
        return iid;
    }
};

/* TODO: */
class IManagedBackbuffer : virtual public IBackbuffer {
public:
    virtual ISemaphore* acquisitionFinishedSemaphore() const noexcept = 0;
    virtual uint64_t acquisitionFinishedSemaphoreValue() const noexcept = 0;
    virtual IFence* acquisitionFinishedFence() const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("cc61aeff-a82d-4f20-9ffb-2c904510ebab");
        return iid;
    }
};

/* TODO: */
class IManagedSwapchain : virtual public ISwapchain {
public:
    virtual Result acquireBackbuffer(bool timeline, IManagedBackbuffer** backbuffer) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("cf1a94ac-7f24-4a8f-ad82-aa301030a868");
        return iid;
    }
};

}