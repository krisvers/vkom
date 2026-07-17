#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

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
