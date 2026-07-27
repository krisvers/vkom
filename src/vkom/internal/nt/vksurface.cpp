#include <vkom/internal/vksurface.hpp>

#include <vkom/platform.hpp>

#include <vkom/internal/vulkan.hpp>

#ifdef VKOM_PLATFORM_FAMILY_NT

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

}

}

#endif