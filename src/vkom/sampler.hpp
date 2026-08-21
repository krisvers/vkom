#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

struct SamplerInfo {
    TexelFilter magnifyFilter;
    TexelFilter minimizeFilter;
    TexelFilter mipmapFilter;
    TexelAddressing addressingU;
    TexelAddressing addressingV;
    TexelAddressing addressingW;
    float anisotropy;
    float mipLODBias;
    float minLOD;
    float maxLOD;
    float borderColor[4];
    bool unnormalizedCoordinates;
};

class ISampler : virtual public IHandled, virtual public ICollected, virtual public IChild {
public:
    virtual void getInfo(SamplerInfo* info) const noexcept = 0;

    static inline IID const& iid() noexcept {
        static IID iid = IID("317e5908-389f-4e7d-af94-a6026ec5efe4");
        return iid;
    }
};

}
