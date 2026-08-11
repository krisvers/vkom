#include <vkom/internal/semaphore.hpp>

#include <stdexcept>
#include <limits>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

namespace vkom {

namespace internal {

VulkanSemaphore::VulkanSemaphore(bool inheritedHandle, IDevice* device, VulkanSemaphoreData const& semaphoreData) : _inheritedHandle(inheritedHandle), _device(device), _adapter(_device->queryInterface<IAdapter>()), _instance(_adapter->queryInterface<IInstance>()), _semaphoreData(semaphoreData) {
    _device->retain();
}

VulkanSemaphore::~VulkanSemaphore() {
    _device->waitIdle();
    _device->disown(IInterface::queryInterface<IChild>());

    if (!_inheritedHandle) {
        _semaphoreData.deviceData.functionPointers.device10.vkDestroySemaphore(_semaphoreData.deviceData.vkDevice, _semaphoreData.vkSemaphore, _semaphoreData.deviceData.adapterData.instanceData.vkAllocationCallbacks);
    }

    _device->release();
}

/* ITimelineSemaphore */
Result VulkanSemaphore::wait(uint64_t value, uint64_t timeout) noexcept {
    if (_semaphoreData.vkSemaphoreType != VK_SEMAPHORE_TYPE_TIMELINE || _semaphoreData.deviceData.functionPointers.device12.vkWaitSemaphores == nullptr) {
        return Result::ErrorUnsupportedFeature;
    }

    VkSemaphoreWaitInfo waitInfo = {};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.semaphoreCount = 1;
    waitInfo.pSemaphores = &_semaphoreData.vkSemaphore;
    waitInfo.pValues = &value;

    return castEnum<Result>(_semaphoreData.deviceData.functionPointers.device12.vkWaitSemaphores(_semaphoreData.deviceData.vkDevice, &waitInfo, timeout));
}

uint64_t VulkanSemaphore::counter() const noexcept {
    if (_semaphoreData.vkSemaphoreType != VK_SEMAPHORE_TYPE_TIMELINE || _semaphoreData.deviceData.functionPointers.device12.vkGetSemaphoreCounterValue == nullptr) {
        return std::numeric_limits<uint64_t>::max();
    }

    uint64_t value;
    if (_semaphoreData.deviceData.functionPointers.device12.vkGetSemaphoreCounterValue(_semaphoreData.deviceData.vkDevice, _semaphoreData.vkSemaphore, &value) == VK_SUCCESS) {
        return std::numeric_limits<uint64_t>::max();
    }

    return value;
}

/* IHandled */
uint64_t VulkanSemaphore::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_semaphoreData.vkSemaphore);
}

ObjectType VulkanSemaphore::handleType() const noexcept {
    return ObjectType::Semaphore;
}

void const* VulkanSemaphore::vkData() const noexcept {
    return reinterpret_cast<void const*>(&_semaphoreData);
}

/* IChild */
IParent* VulkanSemaphore::parent() const noexcept {
    return _device->queryInterface<IParent>();
}

/* IInterface */
void* VulkanSemaphore::queryInterface(IID const& iid) noexcept {
    if (iid == IBase::iid()) {
        return static_cast<IBase*>(this);
    } else if (iid == IHandled::iid()) {
        return static_cast<IHandled*>(this);
    } else if (iid == ICollected::iid()) {
        return static_cast<ICollected*>(this);
    } else if (iid == IParent::iid()) {
        return static_cast<IChild*>(this);
    } else if (iid == ISemaphore::iid()) {
        return static_cast<ISemaphore*>(this);
    } else if (_semaphoreData.vkSemaphoreType == VK_SEMAPHORE_TYPE_TIMELINE && iid == ITimelineSemaphore::iid()) {
        return static_cast<ITimelineSemaphore*>(this);
    }

    return nullptr;
}

}

}