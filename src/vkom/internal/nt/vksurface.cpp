#include <vkom/internal/vksurface.hpp>

#include <vkom/platform.hpp>

#ifdef VKOM_PLATFORM_FAMILY_NT

#include <vkom/internal/vulkan.hpp>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <vulkan/vulkan_win32.h>

namespace vkom {

namespace internal {

bool physicalDeviceQueueFamilySupportsPresentation(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkPhysicalDevice vkPhysicalDevice, uint32_t family) {
    PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>(vkGetInstanceProcAddr(vkInstance, "vkGetPhysicalDeviceWin32PresentationSupportKHR"));
    if (vkGetPhysicalDeviceWin32PresentationSupportKHR == nullptr) {
        return false;
    }

    return vkGetPhysicalDeviceWin32PresentationSupportKHR(vkPhysicalDevice, family);
}


VkResult createSurface(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkAllocationCallbacks const* vkAllocationCallbacks, SurfaceWSIInfo const& info, VkSurfaceKHR& vkSurface) {
    VkResult result = VK_ERROR_UNKNOWN;
    if (info.type == vkom::SurfaceWSIType::Win32) {
        HINSTANCE hinstance = reinterpret_cast<HINSTANCE>(info.displayHandle);
        if (hinstance == nullptr) {
            hinstance = GetModuleHandleA(nullptr);
        }

        VkWin32SurfaceCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hinstance = hinstance;
        createInfo.hwnd = reinterpret_cast<HWND>(info.windowHandle);

        PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(vkGetInstanceProcAddr(vkInstance, "vkCreateWin32SurfaceKHR"));
        if (vkCreateWin32SurfaceKHR != nullptr) {
            result = vkCreateWin32SurfaceKHR(vkInstance, &createInfo, vkAllocationCallbacks, &vkSurface);
        }
    }

    return result;
}

}

}

#endif
