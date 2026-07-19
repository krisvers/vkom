#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#define VKOM_INTERNAL_FUNCPTRS_LOAD(getProcAddr_, handle_, func_, name_) ((func_ = reinterpret_cast<PFN_##func_>(getProcAddr_(handle_, name_))) != nullptr)

namespace vkom {

namespace internal {

struct InstanceFunctionPointers10 {
    PFN_vkDestroyInstance vkDestroyInstance = nullptr;
    PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = nullptr;
    PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties = nullptr;
    PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures = nullptr;
    PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties = nullptr;
    PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties = nullptr;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
    PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties = nullptr;
    PFN_vkCreateDevice vkCreateDevice = nullptr;

    InstanceFunctionPointers10() = default;

    inline bool load(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr) {
        if (!VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkDestroyInstance, "vkDestroyInstance")) {
            return false;
        }

        if (!VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkEnumeratePhysicalDevices, "vkEnumeratePhysicalDevices")) {
            return false;
        }

        if (!VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceProperties, "vkGetPhysicalDeviceProperties")) {
            return false;
        }

        if (!VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceFeatures, "vkGetPhysicalDeviceFeatures")) {
            return false;
        }

        if (!VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceFormatProperties, "vkGetPhysicalDeviceFormatProperties")) {
            return false;
        }

        if (!VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceImageFormatProperties, "vkGetPhysicalDeviceImageFormatProperties")) {
            return false;
        }

        if (!VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceQueueFamilyProperties, "vkGetPhysicalDeviceQueueFamilyProperties")) {
            return false;
        }

        if (!VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkGetPhysicalDeviceMemoryProperties, "vkGetPhysicalDeviceMemoryProperties")) {
            return false;
        }

        if (!VKOM_INTERNAL_FUNCPTRS_LOAD(vkGetInstanceProcAddr, vkInstance, vkCreateDevice, "vkCreateDevice")) {
            return false;
        }

        return true;
    }
};

struct InstanceFunctionPointers11 {

};

struct InstanceFunctionPointers12 {

};

struct DeviceFunctionPointers10 {

};

struct DeviceFunctionPointers11 {

};

struct DeviceFunctionPointers12 {

};

}

}
