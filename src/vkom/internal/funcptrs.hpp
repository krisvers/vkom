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

struct DeviceFunctionPointers10 {
    PFN_vkDestroyDevice vkDestroyDevice = nullptr;

    DeviceFunctionPointers10() = default;

    inline bool load(VkDevice vkDevice, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr) {
        bool successful = VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetDeviceProcAddr, vkDevice, vkDestroyDevice, "vkDestroyDevice");
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

}

}
