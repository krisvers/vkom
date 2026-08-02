#include <vkom/pipeline.hpp>

namespace vkom {

bool IPipeline::supportsInterface(IID const& iid) const noexcept {
    return (iid == IPIPELINE_IID) || INullable::supportsInterface(iid) || IHandled::supportsInterface(iid) || ICollected::supportsInterface(iid) || IChild::supportsInterface(iid);
}

bool IComputePipeline::supportsInterface(IID const& iid) const noexcept {
    return (iid == ICOMPUTEPIPELINE_IID) || IPipeline::supportsInterface(iid);
}

bool IGraphicsPipeline::supportsInterface(IID const& iid) const noexcept {
    return (iid == IGRAPHICSPIPELINE_IID) || IPipeline::supportsInterface(iid);
}

}
