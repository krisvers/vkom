#pragma once

#include "vkom/enums.hpp"
#include <vkom/cmdpasses.hpp>

#include <vkom/cmdencoder.hpp>
#include <vkom/queue.hpp>
#include <vkom/device.hpp>
#include <vkom/adapter.hpp>
#include <vkom/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

class VulkanComputePass final : virtual public IComputePass {
private:
    ICommandEncoder* _encoder = nullptr;
    IQueue* _queue = nullptr;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanCommandPassData _passData;

public:
    VulkanComputePass(ICommandEncoder* encoder, VulkanCommandPassData const& passData);
    ~VulkanComputePass();

    /* IComputePass */
    void dispatch(uint32_t width, uint32_t height, uint32_t depth) noexcept override;

    /* IPass */
    void bindPipeline(IPipeline* pipeline) noexcept override;
    void pushConstants(IPipelineLayout* layout, ShaderStageFlags stages, uint32_t offset, uint32_t size, void const* data) noexcept override;
    void end() noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

class VulkanRenderPass final : virtual public IRenderPass {
private:
    ICommandEncoder* _encoder = nullptr;
    IQueue* _queue = nullptr;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanCommandPassData _passData;

public:
    VulkanRenderPass(ICommandEncoder* encoder, VulkanCommandPassData const& passData);
    ~VulkanRenderPass();

    /* IPass */
    void bindPipeline(IPipeline* pipeline) noexcept override;
    void pushConstants(IPipelineLayout* layout, ShaderStageFlags stages, uint32_t offset, uint32_t size, void const* data) noexcept override;
    void end() noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

}

}
