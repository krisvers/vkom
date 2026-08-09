#include "vkom/enums.hpp"
#include "vkom/surface.hpp"
#include "vulkan/vulkan_core.h"
#include <vkom/internal/vksurface.hpp>

#include <vkom/platform.hpp>

#include <vkom/internal/vulkan.hpp>

#ifdef VKOM_PLATFORM_FAMILY_APPLE

#include <vulkan/vulkan_metal.h>
#include <vulkan/vulkan_macos.h>
#include <vulkan/vulkan_ios.h>

namespace vkom {

namespace internal {

bool physicalDeviceQueueFamilySupportsPresentation(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkPhysicalDevice vkPhysicalDevice, uint32_t family) {
    /* NOTE: i am very unsure whether or not MoltenVK will provide any queues that are non-present compatible
     * however, from my knowledge all provided queue families support present
     *
     * NOTE: this may change with other Vulkan-on-Metal projects like KosmicKrisp
     */
    return true;
}

VkResult createSurface(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkAllocationCallbacks const* vkAllocationCallbacks, SurfaceWSIInfo const& info, VkSurfaceKHR& vkSurface) {
    VkResult result = VK_ERROR_UNKNOWN;
    if (info.type == vkom::SurfaceWSIType::Metal) {
        VkMetalSurfaceCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
        createInfo.pLayer = reinterpret_cast<CAMetalLayer*>(info.windowHandle);

        PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT = reinterpret_cast<PFN_vkCreateMetalSurfaceEXT>(vkGetInstanceProcAddr(vkInstance, "vkCreateMetalSurfaceEXT"));
        if (vkCreateMetalSurfaceEXT != nullptr) {
            result = vkCreateMetalSurfaceEXT(vkInstance, &createInfo, vkAllocationCallbacks, &vkSurface);
        }
    } else if (info.type == vkom::SurfaceWSIType::Cocoa) {
        #ifdef VKOM_PLATFORM_OS_MACOS
        VkMacOSSurfaceCreateInfoMVK createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
        createInfo.pView = reinterpret_cast<void const*>(info.windowHandle);

        PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK = reinterpret_cast<PFN_vkCreateMacOSSurfaceMVK>(vkGetInstanceProcAddr(vkInstance, "vkCreateMacOSSurfaceMVK"));
        if (vkCreateMacOSSurfaceMVK != nullptr) {
            result = vkCreateMacOSSurfaceMVK(vkInstance, &createInfo, vkAllocationCallbacks, &vkSurface);
        }
        #elif defined(VKOM_PLATFORM_OS_IOS)
        VkIOSSurfaceCreateInfoMVK createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
        createInfo.pView = reinterpret_cast<void const*>(info.windowHandle);

        PFN_vkCreateIOSSurfaceMVK vkCreateIOSSurfaceMVK = reinterpret_cast<PFN_vkCreateIOSSurfaceMVK>(vkGetInstanceProcAddr(vkInstance, "vkCreateIOSSurfaceMVK"));
        if (vkCreateIOSSurfaceMVK != nullptr) {
            result = vkCreateIOSSurfaceMVK(vkInstance, &createInfo, vkAllocationCallbacks, &vkSurface);
        }
        #endif
    }

    return result;
}

}

}

#endif
