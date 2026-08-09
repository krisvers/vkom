#pragma once

#include <vkom/surface.hpp>

#include <vkom/internal/vulkan.hpp>

namespace vkom {

namespace internal {

bool physicalDeviceQueueFamilySupportsPresentation(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkPhysicalDevice vkPhysicalDevice, uint32_t family);
VkResult createSurface(VkInstance vkInstance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkAllocationCallbacks const* vkAllocationCallbacks, SurfaceWSIInfo const& info, VkSurfaceKHR& vkSurface);

}

}
