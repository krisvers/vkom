#include <vkom/internal/pipeline.hpp>

#include <vkom/internal/enums.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

VulkanShaderModule::VulkanShaderModule(bool inheritedHandle, IDevice* device, VulkanShaderModuleData const& moduleData) : _inheritedHandle(inheritedHandle), _device(device), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _moduleData(moduleData) {
    _device->retain();
}

VulkanShaderModule::~VulkanShaderModule() {
    _device->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        _moduleData.deviceData.functionPointers.device10.vkDestroyShaderModule(_moduleData.deviceData.vkDevice, _moduleData.vkShaderModule, _moduleData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    _device->release();
}

/* IHandled */
uint64_t VulkanShaderModule::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_moduleData.vkShaderModule);
}

ObjectType VulkanShaderModule::handleType() const noexcept {
    return ObjectType::ShaderModule;
}

void const* VulkanShaderModule::vkData() const noexcept {
    return &_moduleData;
}

/* IChild */
IParent* VulkanShaderModule::parent() const noexcept {
    return _device->queryInterface<IParent>();
}

/* IInterface */
void* VulkanShaderModule::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IShaderModule::iid()) {
        return static_cast<IShaderModule*>(this);
    }

    return nullptr;
}

VulkanPipelineLayout::VulkanPipelineLayout(bool inheritedHandle, IDevice* device, VulkanPipelineLayoutData const& layoutData) : _inheritedHandle(inheritedHandle), _device(device), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _layoutData(layoutData) {
    _device->retain();
}

VulkanPipelineLayout::~VulkanPipelineLayout() {
    _device->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        _layoutData.deviceData.functionPointers.device10.vkDestroyPipelineLayout(_layoutData.deviceData.vkDevice, _layoutData.vkPipelineLayout, _layoutData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    _device->release();
}

/* IHandled */
uint64_t VulkanPipelineLayout::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_layoutData.vkPipelineLayout);
}

ObjectType VulkanPipelineLayout::handleType() const noexcept {
    return ObjectType::PipelineLayout;
}

void const* VulkanPipelineLayout::vkData() const noexcept {
    return &_layoutData;
}

/* IChild */
IParent* VulkanPipelineLayout::parent() const noexcept {
    return _device->queryInterface<IParent>();
}

/* IInterface */
void* VulkanPipelineLayout::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IPipelineLayout::iid()) {
        return static_cast<IPipelineLayout*>(this);
    }

    return nullptr;
}

VulkanPipeline::VulkanPipeline(bool inheritedHandle, IDevice* device, VulkanPipelineData const& pipelineData) : _inheritedHandle(inheritedHandle), _device(device), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _pipelineData(pipelineData) {
    _device->retain();
}

VulkanPipeline::~VulkanPipeline() {
    _device->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        _pipelineData.deviceData.functionPointers.device10.vkDestroyPipeline(_pipelineData.deviceData.vkDevice, _pipelineData.vkPipeline, _pipelineData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    _device->release();
}

/* IHandled */
uint64_t VulkanPipeline::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_pipelineData.vkPipeline);
}

ObjectType VulkanPipeline::handleType() const noexcept {
    return ObjectType::Pipeline;
}

void const* VulkanPipeline::vkData() const noexcept {
    return &_pipelineData;
}

/* IChild */
IParent* VulkanPipeline::parent() const noexcept {
    return _device->queryInterface<IParent>();
}

/* IInterface */
void* VulkanPipeline::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IChild::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IPipeline::iid()) {
        return static_cast<IPipeline*>(this);
    } else if (iid == IGraphicsPipeline::iid()) {
        if (_pipelineData.vkPipelineBindPoint == VK_PIPELINE_BIND_POINT_GRAPHICS) {
            return static_cast<IGraphicsPipeline*>(this);
        }

        return nullptr;
    } else if (iid == IComputePipeline::iid()) {
        if (_pipelineData.vkPipelineBindPoint == VK_PIPELINE_BIND_POINT_COMPUTE) {
            return static_cast<IComputePipeline*>(this);
        }

        return nullptr;
    } else if (iid == IRayTracingPipeline::iid()) {
        if (_pipelineData.vkPipelineBindPoint == VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR) {
            return static_cast<IRayTracingPipeline*>(this);
        }

        return nullptr;
    }

    return nullptr;
}

}

}
