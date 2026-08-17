#pragma once

#include <vkom/platform.hpp>
#include <vkom/enums.hpp>
#include <vkom/object.hpp>

#include <vkom/adapter.hpp>
#include <vkom/surface.hpp>

namespace vkom {

class IInstance;

using InstanceLogCallbackPFN = void(*)(IInstance* instance, void* userData, DebugMessageSeverityFlags severity, DebugMessageTypeFlags types, const char* message);

class IInstance : virtual public IHandled, virtual public ICollected, virtual public IParent, virtual public IDispatchable {
public:
    virtual void setLogCallback(InstanceLogCallbackPFN callback, void* userData) noexcept = 0;
    virtual void log(DebugMessageSeverityFlags severity, DebugMessageTypeFlags types, const char* message) noexcept = 0;

    virtual bool queryExtension(const char* extension) const noexcept = 0;
    virtual uint32_t queryApiVersion() const noexcept = 0;

    virtual IAdapter* enumerateAdapters(uint32_t id) const noexcept = 0;

    virtual Result createSurface(SurfaceWSIInfo const* info, ISurface** surface) noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("b77e41e7-8ad1-435d-979c-4b537954048f");
        return iid;
    }
};

struct InstanceLoaderInfo {
    const char* loaderPath = nullptr;
    void* vkGetInstanceProcAddr = nullptr;
    void* vkInstanceHandle = nullptr;
    void* vkAllocationCallbacks = nullptr;
    void* vkDebugUtilsMessengerCallback = nullptr;
    void* vkDebugUtilsMessengerUserData = nullptr;
};

VKOM_VISIBLE Result createInstance(bool debug, InstanceLoaderInfo const* loaderInfo, IInstance** instance);

}
