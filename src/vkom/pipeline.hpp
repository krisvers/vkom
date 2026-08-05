#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

class IPipeline : virtual public IHandled, virtual public ICollected, virtual public IChild {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("f9fe7204-1096-4456-acdb-09902f90231f");
        return iid;
    }
};

class IComputePipeline : virtual public IPipeline {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("5fdac3eb-47e5-40e6-9940-65be15d578b4");
        return iid;
    }
};

class IGraphicsPipeline : public IPipeline {
public:
    static inline IID const& iid() noexcept {
        static IID iid = IID("7efa9b9b-61da-48b0-911d-4c9f2bb38fdd");
        return iid;
    }
};

}
