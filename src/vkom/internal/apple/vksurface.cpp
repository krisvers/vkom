#include <vkom/internal/vksurface.hpp>

#include <vkom/platform.hpp>

#ifdef VKOM_PLATFORM_FAMILY_APPLE

#include <vkom/enums.hpp>

#include <vkom/internal/vulkan.hpp>

#include <vulkan/vulkan_metal.h>
#include <vulkan/vulkan_macos.h>
#include <vulkan/vulkan_ios.h>

namespace vkom {

namespace internal {

bool physicalDeviceQueueFamilySupportsPresentation(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkPhysicalDevice vkPhysicalDevice, uint32_t family) {
    /* NOTE: i am very unsure whether or not MoltenVK/Kosmic Krisp will provide any queues that are non-present compatible
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
        /* TODO: create Metal layer associated with NSWindow */
        return result;
    }

    return result;
}

}

}

#endif
