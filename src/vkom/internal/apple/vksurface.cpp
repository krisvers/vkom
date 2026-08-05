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

}

}

#endif
