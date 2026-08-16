#pragma once

#include <limits>

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

#include <vkom/surface.hpp>
#include <vkom/swapchain.hpp>

namespace vkom {

inline const uint32_t QUEUE_FAMILY_ANY = std::numeric_limits<uint32_t>::max();

class IQueue;
class IHeap;
class ISemaphore;
class IFence;

class IDevice : virtual public IHandled, virtual public ICollected, virtual public IParent, virtual public IChild, virtual public IDispatchable {
public:
    virtual Result waitIdle() const noexcept = 0;
    virtual bool queryExtension(const char* extension) const noexcept = 0;

    virtual void labelHandle(ObjectType handleType, uint64_t handle, const char* name) noexcept = 0;
    virtual void label(IHandled* handled, const char* name) noexcept = 0;

    virtual Result acquireQueue(uint32_t family, QueueFlags flags, IQueue** queue) noexcept = 0;

    virtual IHeap* defaultHeap() noexcept = 0;
    virtual Result createHeap(BufferUsageFlags bufferUsages, TextureUsageFlags textureUsages, MemoryLocationFlags memoryLocation, IHeap** heap) noexcept = 0;

    virtual Result acquireSemaphore(bool timeline, ISemaphore** semaphore) noexcept = 0;
    virtual Result acquireFence(bool signalled, IFence** fence) noexcept = 0;

    virtual Result createSwapchain(ISurface* surface, SwapchainInfo const* info, ISwapchain** swapchain) noexcept = 0;
    virtual Result createSwapchainAndSurface(SurfaceWSIInfo const* surfaceInfo, SwapchainInfo const* swapchainInfo, ISurface** surface, ISwapchain** swapchain) noexcept = 0;

    template<typename T>
    inline void labelHandle(ObjectType handleType, T handle, const char* name) noexcept {
        labelHandle(handleType, reinterpret_cast<uint64_t>(handle), name);
    }

    static inline IID const& iid() noexcept {
        static IID iid = IID("8af16862-cec4-4dbc-9fe7-c00a6eb9e41c");
        return iid;
    }
};

}
