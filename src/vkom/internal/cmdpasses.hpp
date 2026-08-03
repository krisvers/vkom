#pragma once

#include <vkom/cmdpasses.hpp>

namespace vkom {

namespace internal {

class VulkanComputePass : IComputePass {
public:
    /* IPass */
    void end() noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

class VulkanRenderPass : IRenderPass {
public:
    /* IPass */
    void end() noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

}

}
