#pragma once

#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/vma.hpp>

namespace vkom {

namespace internal {

struct VulkanInstanceFunctionPointers {
    VulkanInstanceFunctionPointers10 instance10;
    VulkanInstanceFunctionPointers11 instance11;
    VulkanInstanceFunctionPointers12 instance12;
    VulkanInstanceFunctionPointersDebugUtilsEXT debugUtilsEXT;
};

struct VulkanInstanceData {
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
    VkInstance vkInstance;
    VkAllocationCallbacks const* vkAllocationCallbacks;
    VulkanInstanceFunctionPointers functionPointers;

    VulkanInstanceData(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkAllocationCallbacks const* vkAllocationCallbacks) : vkInstance(vkInstance), vkGetInstanceProcAddr(vkGetInstanceProcAddr), vkAllocationCallbacks(vkAllocationCallbacks) {
        functionPointers.instance10.load(vkInstance, vkGetInstanceProcAddr);
        functionPointers.instance11.load(vkInstance, vkGetInstanceProcAddr);
        functionPointers.instance12.load(vkInstance, vkGetInstanceProcAddr);
        functionPointers.debugUtilsEXT.load(vkInstance, vkGetInstanceProcAddr);
    }
};

struct VulkanAdapterFunctionPointers {
    VulkanPhysicalDeviceFunctionPointers10 physical10;
    VulkanPhysicalDeviceFunctionPointers11 physical11;
    VulkanPhysicalDeviceFunctionPointersSurfaceKHR surfaceKHR;
};

struct VulkanAdapterData {
    VulkanInstanceData const& instanceData;

    VkPhysicalDevice vkPhysicalDevice;
    VulkanAdapterFunctionPointers functionPointers;

    VulkanAdapterData(VulkanInstanceData const& instanceData, VkPhysicalDevice vkPhysicalDevice) : instanceData(instanceData), vkPhysicalDevice(vkPhysicalDevice) {
        functionPointers.physical10.load(instanceData.vkInstance, instanceData.vkGetInstanceProcAddr);
        functionPointers.physical11.load(instanceData.vkInstance, instanceData.vkGetInstanceProcAddr);
        functionPointers.surfaceKHR.load(instanceData.vkInstance, instanceData.vkGetInstanceProcAddr);
    }
};

struct VulkanDeviceFunctionPointers {
    VulkanDeviceFunctionPointers10 device10;
    VulkanDeviceFunctionPointers11 device11;
    VulkanDeviceFunctionPointers12 device12;
};

struct VulkanDeviceData {
    VulkanAdapterData const& adapterData;

    PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
    VkDevice vkDevice;
    VmaAllocator vmaAllocator;
    VulkanDeviceFunctionPointers functionPointers;

    VulkanDeviceData(VulkanAdapterData const& adapterData, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr, VkDevice vkDevice, VmaAllocator vmaAllocator) : adapterData(adapterData), vkGetDeviceProcAddr(vkGetDeviceProcAddr), vkDevice(vkDevice), vmaAllocator(vmaAllocator) {
        functionPointers.device10.load(vkDevice, vkGetDeviceProcAddr);
        functionPointers.device11.load(vkDevice, vkGetDeviceProcAddr);
        functionPointers.device12.load(vkDevice, vkGetDeviceProcAddr);
    }
};

struct VulkanHeapData {
    VulkanDeviceData const& deviceData;

    VmaPool vmaPool;

    VulkanHeapData(VulkanDeviceData const& deviceData, VmaPool vmaPool) : deviceData(deviceData), vmaPool(vmaPool) {}
};

struct VulkanBufferData {
    VulkanHeapData const& heapData;

    VmaAllocation vmaAllocation;
    VmaAllocationInfo2 vmaAllocationInfo2;
    VkBuffer vkBuffer;

    VulkanBufferData(VulkanDeviceData const& deviceData, VmaAllocation vmaAllocation, VmaAllocationInfo2 vmaAllocationInfo2, VkBuffer vkBuffer) : heapData(heapData), vmaAllocation(vmaAllocation), vmaAllocationInfo2(vmaAllocationInfo2), vkBuffer(vkBuffer) {}
};

struct VulkanTextureData {
    VulkanHeapData const& heapData;

    VmaAllocation vmaAllocation;
    VmaAllocationInfo2 vmaAllocationInfo2;
    VkImage vkImage;

    VulkanTextureData(VulkanDeviceData const& deviceData, VmaAllocation vmaAllocation, VmaAllocationInfo2 vmaAllocationInfo2, VkImage vkImage) : heapData(heapData), vmaAllocation(vmaAllocation), vmaAllocationInfo2(vmaAllocationInfo2), vkImage(vkImage) {}
};

struct VulkanPipelineData {
    VulkanDeviceData const& deviceData;

    VkPipelineBindPoint vkPipelineBindPoint;
    VkPipelineLayout vkPipelineLayout;
    VkPipeline vkPipeline;

    VulkanPipelineData(VulkanDeviceData const& deviceData, VkPipelineBindPoint vkPipelineBindPoint, VkPipelineLayout vkPipelineLayout, VkPipeline vkPipeline) : deviceData(deviceData), vkPipelineBindPoint(vkPipelineBindPoint), vkPipelineLayout(vkPipelineLayout), vkPipeline(vkPipeline) {}
};

struct VulkanQueueFunctionPointers {
    VulkanQueueFunctionPointers10 queue10;
};

struct VulkanQueueData {
    VulkanDeviceData const& deviceData;

    VkQueue vkQueue;
    uint32_t family;
    uint32_t index;
    VulkanQueueFunctionPointers functionPointers;

    VulkanQueueData(VulkanDeviceData const& deviceData, VkQueue vkQueue, uint32_t family, uint32_t index) : deviceData(deviceData), vkQueue(vkQueue), family(family), index(index) {
        functionPointers.queue10.load(deviceData.vkDevice, deviceData.vkGetDeviceProcAddr);
    }
};

struct VulkanCommandBufferFunctionPointers {
    VulkanCommandBufferFunctionPointers10 commandBuffer10;
    VulkanCommandBufferFunctionPointersDebugUtilsEXT debugUtilsEXT;
};

struct VulkanCommandBufferData {
    VulkanQueueData const& queueData;

    VkCommandPool vkCommandPool;
    VkCommandBuffer vkCommandBuffer;
    bool isSecondary;
    VulkanCommandBufferFunctionPointers functionPointers;

    VulkanCommandBufferData(VulkanQueueData const& queueData, VkCommandPool vkCommandPool, VkCommandBuffer vkCommandBuffer, bool isSecondary) : queueData(queueData), vkCommandPool(vkCommandPool), vkCommandBuffer(vkCommandBuffer), isSecondary(isSecondary) {
        functionPointers.commandBuffer10.load(queueData.deviceData.vkDevice, queueData.deviceData.vkGetDeviceProcAddr);
        functionPointers.debugUtilsEXT.load(queueData.deviceData.vkDevice, queueData.deviceData.vkGetDeviceProcAddr);
    }
};

using VulkanCommandEncoderData = VulkanCommandBufferData;
using VulkanCommandBatchData = VulkanCommandBufferData;

struct VulkanCommandPassData {
    VulkanCommandEncoderData const& encoderData;

    VulkanCommandPassData(VulkanCommandEncoderData const& encoderData) : encoderData(encoderData) {}
};

}

}
