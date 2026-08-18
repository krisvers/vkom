#pragma once

#include <vkom/pipeline.hpp>

#include <vkom/device.hpp>
#include <vkom/adapter.hpp>
#include <vkom/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

class VulkanShaderModule final : virtual public IShaderModule, virtual public CollectedByHeap {
private:
    bool _inheritedHandle = false;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanShaderModuleData _moduleData;

public:
    VulkanShaderModule(bool inheritedHandle, IDevice* device, VulkanShaderModuleData const& moduleData);
    ~VulkanShaderModule();

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    void const* vkData() const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

class VulkanPipelineLayout final : virtual public IPipelineLayout, virtual public CollectedByHeap {
private:
    bool _inheritedHandle = false;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanPipelineLayoutData _layoutData;

public:
    VulkanPipelineLayout(bool inheritedHandle, IDevice* device, VulkanPipelineLayoutData const& layoutData);
    ~VulkanPipelineLayout();

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    void const* vkData() const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

/* NOTE: while this class implements IGraphicsPipeline, IComputePipeline, and IRayTracingPipeline,
*   only instances that wrap around a VkPipeline handle created from its specific vkCreate*Pipelines
*   will advertise support (vkCreateGraphicsPipelines will advertise IGraphicsPipeline, ...)
*/

class VulkanPipeline final : virtual public IPipeline, virtual public IGraphicsPipeline, virtual public IComputePipeline, virtual public IRayTracingPipeline, virtual public CollectedByHeap {
private:
    bool _inheritedHandle = false;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanPipelineData _pipelineData;

public:
    VulkanPipeline(bool inheritedHandle, IDevice* device, VulkanPipelineData const& pipelineData);
    ~VulkanPipeline();

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    void const* vkData() const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

}

}
