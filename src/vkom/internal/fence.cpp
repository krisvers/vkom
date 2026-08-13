#include <vkom/internal/fence.hpp>

#include <stdexcept>
#include <limits>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

namespace vkom {

namespace internal {

VulkanFence::VulkanFence(bool inheritedHandle, IDevice* device, VulkanFenceData const& fenceData) : _inheritedHandle(inheritedHandle), _device(device), _adapter(_device->parent<IAdapter>()), _instance(_adapter->parent<IInstance>()), _fenceData(fenceData) {
    _device->retain();
}

VulkanFence::~VulkanFence() {
    wait();
    _device->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        _fenceData.deviceData.functionPointers.device10.vkDestroyFence(_fenceData.deviceData.vkDevice, _fenceData.vkFence, _fenceData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    _device->release();
}

/* IFence */
Result VulkanFence::reset() noexcept {
    return castEnum<Result>(_fenceData.deviceData.functionPointers.device10.vkResetFences(_fenceData.deviceData.vkDevice, 1, &_fenceData.vkFence));
}

bool VulkanFence::status() const noexcept {
    return (_fenceData.deviceData.functionPointers.device10.vkGetFenceStatus(_fenceData.deviceData.vkDevice, _fenceData.vkFence) == VK_SUCCESS);
}

Result VulkanFence::wait(uint64_t timeout) noexcept {
    return castEnum<Result>(_fenceData.deviceData.functionPointers.device10.vkWaitForFences(_fenceData.deviceData.vkDevice, 1, &_fenceData.vkFence, true, timeout));
}

/* IHandled */
uint64_t VulkanFence::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_fenceData.vkFence);
}

ObjectType VulkanFence::handleType() const noexcept {
    return ObjectType::Fence;
}

void const* VulkanFence::vkData() const noexcept {
    return reinterpret_cast<void const*>(&_fenceData);
}

/* IChild */
IParent* VulkanFence::parent() const noexcept {
    return _device->queryInterface<IParent>();
}

/* IInterface */
void* VulkanFence::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IParent::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == IFence::iid()) {
        return static_cast<IFence*>(this);
    }

    return nullptr;
}

}

}