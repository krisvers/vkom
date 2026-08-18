#include <vkom/internal/cmdpasses.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/cmdencoder.hpp>
#include <vkom/internal/queue.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

VulkanComputePass::VulkanComputePass(ICommandEncoder* encoder, VulkanCommandPassData const& passData) : _encoder(encoder), _queue(_encoder->parent<IQueue>()), _device(_queue->parent<IDevice>()), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _passData(passData) {
    encoder->retain();
}

VulkanComputePass::~VulkanComputePass() {
    _encoder->disown(IInterface::queryInterface<IChild>());
    _encoder->release();
}

/* IComputePass */
void VulkanComputePass::dispatch(uint32_t width, uint32_t height, uint32_t depth) noexcept {
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdDispatch(_passData.encoderData.vkCommandBuffer, width, height, depth);
}

/* IPass */
void VulkanComputePass::bindPipeline(IPipeline* pipeline) noexcept {
    if (pipeline->queryInterface<IComputePipeline>() == nullptr) {
        /* TODO: warn */
        return;
    }

    VkPipeline vkPipeline = pipeline->handle<VkPipeline>();
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdBindPipeline(_passData.encoderData.vkCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vkPipeline);
}

void VulkanComputePass::pushConstants(IPipelineLayout* layout, ShaderStageFlags stages, uint32_t offset, uint32_t size, void const* data) noexcept {
    VkPipelineLayout vkPipelineLayout = layout->handle<VkPipelineLayout>();
    _passData.encoderData.functionPointers.commandBuffer10.vkCmdPushConstants(_passData.encoderData.vkCommandBuffer, vkPipelineLayout, castEnum<VkShaderStageFlags>(stages), offset, size, data);
}

void VulkanComputePass::end() noexcept {
    delete this;
}

/* IChild */
IParent* VulkanComputePass::parent() const noexcept {
    return _encoder->queryInterface<IParent>();
}

/* IInterface */
void* VulkanComputePass::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IPass::iid()) {
        return static_cast<IPass*>(this);
    } else if (iid == IComputePass::iid()) {
        return static_cast<IComputePass*>(this);
    }

    return nullptr;
}

}

}
