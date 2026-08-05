#include <vkom/internal/vksurface.hpp>

#include <vkom/platform.hpp>

#include <vkom/internal/vulkan.hpp>

#if defined(VKOM_PLATFORM_FAMILY_UNIX) && !defined(VKOM_PLATFORM_FAMILY_APPLE)

#include <vulkan/vulkan_xlib.h>
#include <vulkan/vulkan_xcb.h>
#include <vulkan/vulkan_wayland.h>

namespace vkom {

namespace internal {

bool physicalDeviceQueueFamilySupportsPresentation(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkPhysicalDevice vkPhysicalDevice, uint32_t family) {
    PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR vkGetPhysicalDeviceWaylandPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR>(vkGetInstanceProcAddr(vkInstance, "PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR"));
    if (vkGetPhysicalDeviceWaylandPresentationSupportKHR != nullptr && vkGetPhysicalDeviceWaylandPresentationSupportKHR(vkPhysicalDevice, family)) {
        return true;
    }

    PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR vkGetPhysicalDeviceXcbPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR>(vkGetInstanceProcAddr(vkInstance, "PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR"));
    if (vkGetPhysicalDeviceXcbPresentationSupportKHR != nullptr && vkGetPhysicalDeviceXcbPresentationSupportKHR(vkPhysicalDevice, family)) {
        return true;
    }

    PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR vkGetPhysicalDeviceXlibPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR>(vkGetInstanceProcAddr(vkInstance, "PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR"));
    if (vkGetPhysicalDeviceXlibPresentationSupportKHR != nullptr && vkGetPhysicalDeviceXlibPresentationSupportKHR(vkPhysicalDevice, family)) {
        return true;
    }

    return false;
}

}

}

#endif
