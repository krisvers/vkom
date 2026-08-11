#include <vkom/internal/vksurface.hpp>

#include <vkom/platform.hpp>

#include <vkom/internal/vulkan.hpp>

#if defined(VKOM_PLATFORM_FAMILY_UNIX) && !defined(VKOM_PLATFORM_FAMILY_APPLE)

struct Display;
using Window = unsigned int;

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

VkResult createSurface(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkAllocationCallbacks const* vkAllocationCallbacks, SurfaceWSIInfo const& info, VkSurfaceKHR& vkSurface) {
    VkResult result = VK_ERROR_UNKNOWN;
    if (info.type == vkom::SurfaceWSIType::Xlib) {
        VkXlibSurfaceCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        createInfo.dpy = reinterpret_cast<Display*>(info.displayHandle);
        createInfo.window = reinterpret_cast<Window>(info.windowHandle);

        PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(vkGetInstanceProcAddr(vkInstance, "vkCreateXlibSurfaceKHR"));
        if (vkCreateXlibSurfaceKHR != nullptr) {
            result = vkCreateXlibSurfaceKHR(vkInstance, &createInfo, vkAllocationCallbacks, &vkSurface);
        }
    } else if (info.type == vkom::SurfaceWSIType::Xcb) {
        VkXcbSurfaceCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        createInfo.connection = reinterpret_cast<xcb_connection_t*>(info.displayHandle);
        createInfo.window = reinterpret_cast<xcb_window_t>(info.windowHandle);

        PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR = reinterpret_cast<PFN_vkCreateXcbSurfaceKHR>(vkGetInstanceProcAddr(vkInstance, "vkCreateXcbSurfaceKHR"));
        if (vkCreateXcbSurfaceKHR != nullptr) {
            result = vkCreateXcbSurfaceKHR(vkInstance, &createInfo, vkAllocationCallbacks, &vkSurface);
        }
    } else if (info.type == vkom::SurfaceWSIType::Wayland) {
        VkWaylandSurfaceCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
        createInfo.display = reinterpret_cast<struct wl_display*>(info.displayHandle);
        createInfo.surface = reinterpret_cast<struct wl_surface*>(info.windowHandle);

        PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR = reinterpret_cast<PFN_vkCreateWaylandSurfaceKHR>(vkGetInstanceProcAddr(vkInstance, "vkCreateWaylandSurfaceKHR"));
        if (vkCreateWaylandSurfaceKHR != nullptr) {
            result = vkCreateWaylandSurfaceKHR(vkInstance, &createInfo, vkAllocationCallbacks, &vkSurface);
        }
    }

    return result;
}

}

}

#endif
