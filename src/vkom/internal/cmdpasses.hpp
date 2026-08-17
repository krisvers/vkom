#pragma once

#include <vkom/cmdpasses.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vkdata.hpp>

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
