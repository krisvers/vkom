#pragma once

#include <vkom/internal/vulkan.hpp>

#define VKOM_INTERNAL_FUNCPTRS_LOAD(getProcAddr_, handle_, func_, name_) ((func_ = reinterpret_cast<PFN_##func_>(getProcAddr_(handle_, name_))) != nullptr)

namespace vkom {

namespace internal {

struct InstanceFunctionPointers10 {
    PFN_vkDestroyInstance vkDestroyInstance = nullptr;
    PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = nullptr;

    InstanceFunctionPointers10() = default;

    inline bool load(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr) {
        bool successful = VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkDestroyInstance, "vkDestroyInstance");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkEnumeratePhysicalDevices, "vkEnumeratePhysicalDevices");
        return successful;
    }
};

struct InstanceFunctionPointers11 {
    InstanceFunctionPointers11() = default;

    inline bool load(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr) {
        bool successful = false;
        return successful;
    }
};

struct InstanceFunctionPointers12 {
    InstanceFunctionPointers12() = default;

    inline bool load(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr) {
        bool successful = false;
        return successful;
    }
};

struct InstanceFunctionPointersDebugUtilsEXT {
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;
    PFN_vkSubmitDebugUtilsMessageEXT vkSubmitDebugUtilsMessageEXT = nullptr;

    InstanceFunctionPointersDebugUtilsEXT() = default;

    inline bool load(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr) {
        bool successful = VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkCreateDebugUtilsMessengerEXT, "vkCreateDebugUtilsMessengerEXT");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkDestroyDebugUtilsMessengerEXT, "vkDestroyDebugUtilsMessengerEXT");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkSubmitDebugUtilsMessageEXT, "vkSubmitDebugUtilsMessageEXT");
        return successful;
    }
};

struct PhysicalDeviceFunctionPointers10 {
    PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties = nullptr;
    PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures = nullptr;
    PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties = nullptr;
    PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties = nullptr;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
    PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties = nullptr;
    PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties = nullptr;
    PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties = nullptr;
    PFN_vkCreateDevice vkCreateDevice = nullptr;

    PhysicalDeviceFunctionPointers10() = default;

    inline bool load(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr) {
        bool successful = VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceProperties, "vkGetPhysicalDeviceProperties");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceFeatures, "vkGetPhysicalDeviceFeatures");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceFormatProperties, "vkGetPhysicalDeviceFormatProperties");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceImageFormatProperties, "vkGetPhysicalDeviceImageFormatProperties");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceQueueFamilyProperties, "vkGetPhysicalDeviceQueueFamilyProperties");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceMemoryProperties, "vkGetPhysicalDeviceMemoryProperties");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkEnumerateDeviceExtensionProperties, "vkEnumerateDeviceExtensionProperties");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkEnumerateDeviceLayerProperties, "vkEnumerateDeviceLayerProperties");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkCreateDevice, "vkCreateDevice");
        return successful;
    }
};

struct PhysicalDeviceFunctionPointers11 {
    PFN_vkGetPhysicalDeviceExternalBufferProperties vkGetPhysicalDeviceExternalBufferProperties = nullptr;
    PFN_vkGetPhysicalDeviceExternalFenceProperties vkGetPhysicalDeviceExternalFenceProperties = nullptr;
    PFN_vkGetPhysicalDeviceExternalSemaphoreProperties vkGetPhysicalDeviceExternalSemaphoreProperties = nullptr;
    PFN_vkGetPhysicalDeviceFeatures2 vkGetPhysicalDeviceFeatures2 = nullptr;
    PFN_vkGetPhysicalDeviceFormatProperties2 vkGetPhysicalDeviceFormatProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceImageFormatProperties2 vkGetPhysicalDeviceImageFormatProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceMemoryProperties2 vkGetPhysicalDeviceMemoryProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceProperties2 vkGetPhysicalDeviceProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2 vkGetPhysicalDeviceQueueFamilyProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 vkGetPhysicalDeviceSparseImageFormatProperties2 = nullptr;

    PhysicalDeviceFunctionPointers11() = default;

    inline bool load(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr) {
        bool successful = VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceExternalBufferProperties, "vkGetPhysicalDeviceExternalBufferProperties") || VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceExternalBufferProperties, "vkGetPhysicalDeviceExternalBufferPropertiesKHR");
        successful = successful && (VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceExternalFenceProperties, "vkGetPhysicalDeviceExternalFenceProperties") || VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceExternalFenceProperties, "vkGetPhysicalDeviceExternalFencePropertiesKHR"));
        successful = successful && (VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceExternalSemaphoreProperties, "vkGetPhysicalDeviceExternalSemaphoreProperties") || VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceExternalSemaphoreProperties, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR"));
        successful = successful && (VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceFeatures2, "vkGetPhysicalDeviceFeatures2") || VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceFeatures2, "vkGetPhysicalDeviceFeatures2KHR"));
        successful = successful && (VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceFormatProperties2, "vkGetPhysicalDeviceFormatProperties2") || VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceFormatProperties2, "vkGetPhysicalDeviceFormatProperties2KHR"));
        successful = successful && (VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceImageFormatProperties2, "vkGetPhysicalDeviceImageFormatProperties2") || VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceImageFormatProperties2, "vkGetPhysicalDeviceImageFormatProperties2KHR"));
        successful = successful && (VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceMemoryProperties2, "vkGetPhysicalDeviceMemoryProperties2") || VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceMemoryProperties2, "vkGetPhysicalDeviceMemoryProperties2KHR"));
        successful = successful && (VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceProperties2, "vkGetPhysicalDeviceProperties2") || VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceProperties2, "vkGetPhysicalDeviceProperties2KHR"));
        successful = successful && (VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceQueueFamilyProperties2, "vkGetPhysicalDeviceQueueFamilyProperties2") || VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceQueueFamilyProperties2, "vkGetPhysicalDeviceQueueFamilyProperties2KHR"));
        successful = successful && (VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceSparseImageFormatProperties2, "vkGetPhysicalDeviceSparseImageFormatProperties2") || VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceSparseImageFormatProperties2, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR"));
        return successful;
    }
};

struct PhysicalDeviceFunctionPointersSurfaceKHR {
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;

    PhysicalDeviceFunctionPointersSurfaceKHR() = default;

    inline bool load(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr) {
        bool successful = VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceSurfaceCapabilitiesKHR, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceSurfaceFormatsKHR, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceSurfacePresentModesKHR, "vkGetPhysicalDeviceSurfacePresentModesKHR");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceSurfaceSupportKHR, "vkGetPhysicalDeviceSurfaceSupportKHR");
        return successful;
    }
};

struct DeviceFunctionPointers10 {
    PFN_vkDestroyDevice vkDestroyDevice = nullptr;
    PFN_vkGetDeviceQueue vkGetDeviceQueue = nullptr;
    PFN_vkDeviceWaitIdle vkDeviceWaitIdle = nullptr;
    PFN_vkAllocateMemory vkAllocateMemory = nullptr;
    PFN_vkFreeMemory vkFreeMemory = nullptr;
    PFN_vkMapMemory vkMapMemory = nullptr;
    PFN_vkUnmapMemory vkUnmapMemory = nullptr;
    PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges = nullptr;
    PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges = nullptr;
    PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment = nullptr;
    PFN_vkBindBufferMemory vkBindBufferMemory = nullptr;
    PFN_vkBindImageMemory vkBindImageMemory = nullptr;
    PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements = nullptr;
    PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements = nullptr;
    PFN_vkGetImageSparseMemoryRequirements vkGetImageSparseMemoryRequirements = nullptr;
    PFN_vkCreateFence vkCreateFence = nullptr;
    PFN_vkDestroyFence vkDestroyFence = nullptr;
    PFN_vkResetFences vkResetFences = nullptr;
    PFN_vkGetFenceStatus vkGetFenceStatus = nullptr;
    PFN_vkWaitForFences vkWaitForFences = nullptr;
    PFN_vkCreateSemaphore vkCreateSemaphore = nullptr;
    PFN_vkDestroySemaphore vkDestroySemaphore = nullptr;
    PFN_vkCreateQueryPool vkCreateQueryPool = nullptr;
    PFN_vkDestroyQueryPool vkDestroyQueryPool = nullptr;
    PFN_vkGetQueryPoolResults vkGetQueryPoolResults = nullptr;
    PFN_vkCreateBuffer vkCreateBuffer = nullptr;
    PFN_vkDestroyBuffer vkDestroyBuffer = nullptr;
    PFN_vkCreateImage vkCreateImage = nullptr;
    PFN_vkDestroyImage vkDestroyImage = nullptr;
    PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout = nullptr;
    PFN_vkCreateImageView vkCreateImageView = nullptr;
    PFN_vkDestroyImageView vkDestroyImageView = nullptr;
    PFN_vkCreateCommandPool vkCreateCommandPool = nullptr;
    PFN_vkDestroyCommandPool vkDestroyCommandPool = nullptr;
    PFN_vkResetCommandPool vkResetCommandPool = nullptr;
    PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers = nullptr;
    PFN_vkFreeCommandBuffers vkFreeCommandBuffers = nullptr;
    PFN_vkCreateEvent vkCreateEvent = nullptr;
    PFN_vkDestroyEvent vkDestroyEvent = nullptr;
    PFN_vkGetEventStatus vkGetEventStatus = nullptr;
    PFN_vkSetEvent vkSetEvent = nullptr;
    PFN_vkResetEvent vkResetEvent = nullptr;
    PFN_vkCreateBufferView vkCreateBufferView = nullptr;
    PFN_vkDestroyBufferView vkDestroyBufferView = nullptr;
    PFN_vkCreateShaderModule vkCreateShaderModule = nullptr;
    PFN_vkDestroyShaderModule vkDestroyShaderModule = nullptr;
    PFN_vkCreatePipelineCache vkCreatePipelineCache = nullptr;
    PFN_vkDestroyPipelineCache vkDestroyPipelineCache = nullptr;
    PFN_vkGetPipelineCacheData vkGetPipelineCacheData = nullptr;
    PFN_vkMergePipelineCaches vkMergePipelineCaches = nullptr;
    PFN_vkCreateComputePipelines vkCreateComputePipelines = nullptr;
    PFN_vkDestroyPipeline vkDestroyPipeline = nullptr;
    PFN_vkCreatePipelineLayout vkCreatePipelineLayout = nullptr;
    PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout = nullptr;
    PFN_vkCreateSampler vkCreateSampler = nullptr;
    PFN_vkDestroySampler vkDestroySampler = nullptr;
    PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout = nullptr;
    PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout = nullptr;
    PFN_vkCreateDescriptorPool vkCreateDescriptorPool = nullptr;
    PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool = nullptr;
    PFN_vkResetDescriptorPool vkResetDescriptorPool = nullptr;
    PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets = nullptr;
    PFN_vkFreeDescriptorSets vkFreeDescriptorSets = nullptr;
    PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets = nullptr;
    PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines = nullptr;
    PFN_vkCreateFramebuffer vkCreateFramebuffer = nullptr;
    PFN_vkDestroyFramebuffer vkDestroyFramebuffer = nullptr;
    PFN_vkCreateRenderPass vkCreateRenderPass = nullptr;
    PFN_vkDestroyRenderPass vkDestroyRenderPass = nullptr;
    PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity = nullptr;

    DeviceFunctionPointers10() = default;

    inline bool load(VkDevice vkDevice, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr) {
        bool successful = VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyDevice, "vkDestroyDevice");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkGetDeviceQueue, "vkGetDeviceQueue");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDeviceWaitIdle, "vkDeviceWaitIdle");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkAllocateMemory, "vkAllocateMemory");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkFreeMemory, "vkFreeMemory");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkMapMemory, "vkMapMemory");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkUnmapMemory, "vkUnmapMemory");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkFlushMappedMemoryRanges, "vkFlushMappedMemoryRanges");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkInvalidateMappedMemoryRanges, "vkInvalidateMappedMemoryRanges");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkGetDeviceMemoryCommitment, "vkGetDeviceMemoryCommitment");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkBindBufferMemory, "vkBindBufferMemory");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkBindImageMemory, "vkBindImageMemory");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkGetBufferMemoryRequirements, "vkGetBufferMemoryRequirements");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkGetImageMemoryRequirements, "vkGetImageMemoryRequirements");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkGetImageSparseMemoryRequirements, "vkGetImageSparseMemoryRequirements");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateFence, "vkCreateFence");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyFence, "vkDestroyFence");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkResetFences, "vkResetFences");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkGetFenceStatus, "vkGetFenceStatus");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkWaitForFences, "vkWaitForFences");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateSemaphore, "vkCreateSemaphore");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroySemaphore, "vkDestroySemaphore");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateQueryPool, "vkCreateQueryPool");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyQueryPool, "vkDestroyQueryPool");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkGetQueryPoolResults, "vkGetQueryPoolResults");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateBuffer, "vkCreateBuffer");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyBuffer, "vkDestroyBuffer");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateImage, "vkCreateImage");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyImage, "vkDestroyImage");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkGetImageSubresourceLayout, "vkGetImageSubresourceLayout");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateImageView, "vkCreateImageView");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyImageView, "vkDestroyImageView");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateCommandPool, "vkCreateCommandPool");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyCommandPool, "vkDestroyCommandPool");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkResetCommandPool, "vkResetCommandPool");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkAllocateCommandBuffers, "vkAllocateCommandBuffers");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkFreeCommandBuffers, "vkFreeCommandBuffers");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateEvent, "vkCreateEvent");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyEvent, "vkDestroyEvent");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkGetEventStatus, "vkGetEventStatus");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkSetEvent, "vkSetEvent");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkResetEvent, "vkResetEvent");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateBufferView, "vkCreateBufferView");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyBufferView, "vkDestroyBufferView");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateShaderModule, "vkCreateShaderModule");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyShaderModule, "vkDestroyShaderModule");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreatePipelineCache, "vkCreatePipelineCache");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyPipelineCache, "vkDestroyPipelineCache");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkGetPipelineCacheData, "vkGetPipelineCacheData");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkMergePipelineCaches, "vkMergePipelineCaches");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateComputePipelines, "vkCreateComputePipelines");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyPipeline, "vkDestroyPipeline");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreatePipelineLayout, "vkCreatePipelineLayout");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyPipelineLayout, "vkDestroyPipelineLayout");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateSampler, "vkCreateSampler");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroySampler, "vkDestroySampler");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateDescriptorSetLayout, "vkCreateDescriptorSetLayout");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyDescriptorSetLayout, "vkDestroyDescriptorSetLayout");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateDescriptorPool, "vkCreateDescriptorPool");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyDescriptorPool, "vkDestroyDescriptorPool");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkResetDescriptorPool, "vkResetDescriptorPool");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkAllocateDescriptorSets, "vkAllocateDescriptorSets");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkFreeDescriptorSets, "vkFreeDescriptorSets");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkUpdateDescriptorSets, "vkUpdateDescriptorSets");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateGraphicsPipelines, "vkCreateGraphicsPipelines");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateFramebuffer, "vkCreateFramebuffer");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyFramebuffer, "vkDestroyFramebuffer");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCreateRenderPass, "vkCreateRenderPass");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyRenderPass, "vkDestroyRenderPass");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkGetRenderAreaGranularity, "vkGetRenderAreaGranularity");
        return successful;
    }
};

struct DeviceFunctionPointers11 {
    DeviceFunctionPointers11() = default;

    inline bool load(VkDevice vkDevice, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr) {
        bool successful = false;
        return successful;
    }
};

struct DeviceFunctionPointers12 {
    DeviceFunctionPointers12() = default;

    inline bool load(VkDevice vkDevice, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr) {
        bool successful = false;
        return successful;
    }
};

struct QueueFunctionPointers10 {
    PFN_vkQueueSubmit vkQueueSubmit = nullptr;
    PFN_vkQueueWaitIdle vkQueueWaitIdle = nullptr;
    PFN_vkQueueBindSparse vkQueueBindSparse = nullptr;

    QueueFunctionPointers10() = default;

    inline bool load(VkDevice vkDevice, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr) {
        bool successful = VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkQueueSubmit, "vkQueueSubmit");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkQueueWaitIdle, "vkQueueWaitIdle");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkQueueBindSparse, "vkQueueBindSparse");
        return successful;
    }
};

struct QueueFunctionPointersSwapchainKHR {
    PFN_vkQueuePresentKHR vkQueuePresentKHR = nullptr;

    QueueFunctionPointersSwapchainKHR() = default;

    inline bool load(VkDevice vkDevice, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr) {
        bool successful = VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkQueuePresentKHR, "vkQueuePresentKHR");
        return successful;
    }
};

struct CommandBufferFunctionPointers10 {
    PFN_vkBeginCommandBuffer vkBeginCommandBuffer = nullptr;
    PFN_vkEndCommandBuffer vkEndCommandBuffer = nullptr;
    PFN_vkResetCommandBuffer vkResetCommandBuffer = nullptr;

    PFN_vkCmdCopyBuffer vkCmdCopyBuffer = nullptr;
    PFN_vkCmdCopyImage vkCmdCopyImage = nullptr;
    PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage = nullptr;
    PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer = nullptr;
    PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer = nullptr;
    PFN_vkCmdFillBuffer vkCmdFillBuffer = nullptr;
    PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier = nullptr;
    PFN_vkCmdBeginQuery vkCmdBeginQuery = nullptr;
    PFN_vkCmdEndQuery vkCmdEndQuery = nullptr;
    PFN_vkCmdResetQueryPool vkCmdResetQueryPool = nullptr;
    PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp = nullptr;
    PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults = nullptr;
    PFN_vkCmdExecuteCommands vkCmdExecuteCommands = nullptr;

    PFN_vkCmdBindPipeline vkCmdBindPipeline = nullptr;
    PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets = nullptr;
    PFN_vkCmdClearColorImage vkCmdClearColorImage = nullptr;
    PFN_vkCmdDispatch vkCmdDispatch = nullptr;
    PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect = nullptr;
    PFN_vkCmdSetEvent vkCmdSetEvent = nullptr;
    PFN_vkCmdResetEvent vkCmdResetEvent = nullptr;
    PFN_vkCmdWaitEvents vkCmdWaitEvents = nullptr;
    PFN_vkCmdPushConstants vkCmdPushConstants = nullptr;

    PFN_vkCmdSetViewport vkCmdSetViewport = nullptr;
    PFN_vkCmdSetScissor vkCmdSetScissor = nullptr;
    PFN_vkCmdSetLineWidth vkCmdSetLineWidth = nullptr;
    PFN_vkCmdSetDepthBias vkCmdSetDepthBias = nullptr;
    PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants = nullptr;
    PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds = nullptr;
    PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask = nullptr;
    PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask = nullptr;
    PFN_vkCmdSetStencilReference vkCmdSetStencilReference = nullptr;
    PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer = nullptr;
    PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers = nullptr;
    PFN_vkCmdDraw vkCmdDraw = nullptr;
    PFN_vkCmdDrawIndexed vkCmdDrawIndexed = nullptr;
    PFN_vkCmdDrawIndirect vkCmdDrawIndirect = nullptr;
    PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect = nullptr;
    PFN_vkCmdBlitImage vkCmdBlitImage = nullptr;
    PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage = nullptr;
    PFN_vkCmdClearAttachments vkCmdClearAttachments = nullptr;
    PFN_vkCmdResolveImage vkCmdResolveImage = nullptr;
    PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass = nullptr;
    PFN_vkCmdNextSubpass vkCmdNextSubpass = nullptr;
    PFN_vkCmdEndRenderPass vkCmdEndRenderPass = nullptr;

    CommandBufferFunctionPointers10() = default;

    inline bool load(VkDevice vkDevice, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr) {
        bool successful = VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkBeginCommandBuffer, "vkBeginCommandBuffer");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkEndCommandBuffer, "vkEndCommandBuffer");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkResetCommandBuffer, "vkResetCommandBuffer");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdCopyBuffer, "vkCmdCopyBuffer");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdCopyImage, "vkCmdCopyImage");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdCopyBufferToImage, "vkCmdCopyBufferToImage");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdCopyImageToBuffer, "vkCmdCopyImageToBuffer");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdUpdateBuffer, "vkCmdUpdateBuffer");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdFillBuffer, "vkCmdFillBuffer");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdPipelineBarrier, "vkCmdPipelineBarrier");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdBeginQuery, "vkCmdBeginQuery");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdEndQuery, "vkCmdEndQuery");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdResetQueryPool, "vkCmdResetQueryPool");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdWriteTimestamp, "vkCmdWriteTimestamp");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdCopyQueryPoolResults, "vkCmdCopyQueryPoolResults");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdExecuteCommands, "vkCmdExecuteCommands");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdBindPipeline, "vkCmdBindPipeline");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdBindDescriptorSets, "vkCmdBindDescriptorSets");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdClearColorImage, "vkCmdClearColorImage");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdDispatch, "vkCmdDispatch");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdDispatchIndirect, "vkCmdDispatchIndirect");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdSetEvent, "vkCmdSetEvent");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdResetEvent, "vkCmdResetEvent");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdWaitEvents, "vkCmdWaitEvents");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdPushConstants, "vkCmdPushConstants");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdSetViewport, "vkCmdSetViewport");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdSetScissor, "vkCmdSetScissor");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdSetLineWidth, "vkCmdSetLineWidth");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdSetDepthBias, "vkCmdSetDepthBias");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdSetBlendConstants, "vkCmdSetBlendConstants");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdSetDepthBounds, "vkCmdSetDepthBounds");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdSetStencilCompareMask, "vkCmdSetStencilCompareMask");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdSetStencilWriteMask, "vkCmdSetStencilWriteMask");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdSetStencilReference, "vkCmdSetStencilReference");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdBindIndexBuffer, "vkCmdBindIndexBuffer");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdBindVertexBuffers, "vkCmdBindVertexBuffers");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdDraw, "vkCmdDraw");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdDrawIndexed, "vkCmdDrawIndexed");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdDrawIndirect, "vkCmdDrawIndirect");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdDrawIndexedIndirect, "vkCmdDrawIndexedIndirect");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdBlitImage, "vkCmdBlitImage");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdClearDepthStencilImage, "vkCmdClearDepthStencilImage");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdClearAttachments, "vkCmdClearAttachments");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdResolveImage, "vkCmdResolveImage");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdBeginRenderPass, "vkCmdBeginRenderPass");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdNextSubpass, "vkCmdNextSubpass");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdEndRenderPass, "vkCmdEndRenderPass");
        return successful;
    }
};

struct CommandBufferFunctionPointersDebugUtilsEXT {
    PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT = nullptr;
    PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT = nullptr;
    PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT = nullptr;

    inline bool load(VkDevice vkDevice, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr) {
        bool successful = VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdBeginDebugUtilsLabelEXT, "vkCmdBeginDebugUtilsLabelEXT");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdEndDebugUtilsLabelEXT, "vkCmdEndDebugUtilsLabelEXT");
        successful = successful && VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkCmdInsertDebugUtilsLabelEXT, "vkCmdInsertDebugUtilsLabelEXT");
        return successful;
    }
};

}

}
