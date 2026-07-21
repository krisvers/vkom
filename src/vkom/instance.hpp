#pragma once

#include <vkom/platform.hpp>
#include <vkom/enums.hpp>
#include <vkom/object.hpp>
#include <vkom/adapter.hpp>

namespace vkom {

inline const IID IINSTANCE_IID = IID("b77e41e7-8ad1-435d-979c-4b537954048f");

class IInstance : public INullable, public IHandled, public ICollected, public IParent, public IDispatchable {
public:
    virtual IAdapter* enumerateAdapters(uint32_t id) const noexcept = 0;

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

struct InstanceLoaderInfo {
    const char* loaderPath = nullptr;
    void* vkGetInstanceProcAddr = nullptr;
    void* vkInstanceHandle = nullptr;
    void* vkAllocationCallbacks = nullptr;
};

VKOM_VISIBLE Result createInstance(bool debug, InstanceLoaderInfo const* loaderInfo, IInstance** instance);

}
