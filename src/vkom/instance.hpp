#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

inline const IID IINSTANCE_IID = IID("b77e41e7-8ad1-435d-979c-4b537954048f");

class IInstance : virtual public INullable, virtual public IHandled, virtual public ICollected, virtual public IParent, virtual public IDispatchable {
public:


    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

struct InstanceLoaderInfo {
    const char* loaderPath = nullptr;
    void* vkGetInstanceProcAddr = nullptr;
    void* vkInstanceHandle = nullptr;
};

Result createInstance(bool debug, InstanceLoaderInfo const* loaderInfo, IInstance** instance);

}
