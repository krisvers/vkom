#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>

namespace vkom {

inline const IID IPIPELINE_IID = IID("f9fe7204-1096-4456-acdb-09902f90231f");

class IPipeline : public INullable, public IHandled, public ICollected, public IChild {
public:
    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

inline const IID ICOMPUTEPIPELINE_IID = IID("5fdac3eb-47e5-40e6-9940-65be15d578b4");

class IComputePipeline : public IPipeline {
public:
    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

inline const IID IGRAPHICSPIPELINE_IID = IID("7efa9b9b-61da-48b0-911d-4c9f2bb38fdd");

class IGraphicsPipeline : public IPipeline {
public:
    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

}