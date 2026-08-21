#pragma once

#include "vulkan/vulkan_core.h"
#include <type_traits>
#include <limits>

#include <vkom/enums.hpp>

#include <vkom/internal/vulkan.hpp>

#ifdef VKOM_INTERNAL_ENUM_IMPLICIT_OPERATOR_CAST
#define VKOM_INTERNAL_ENUM_DEFINE_CAST(vkT_, vkomT_) \
inline vkT_ castEnum(vkomT_ v) { \
    return static_cast<vkT_>(static_cast<std::underlying_type_t<vkomT_>>(v)); \
} \
inline vkomT_ castEnum(vkT_ v) { \
    return static_cast<vkomT_>(static_cast<std::underlying_type_t<vkT_>>(v)); \
} \
inline operator vkT_(vkomT_ v) { \
    return castEnum(v); \
} \
inline operator vkomT_(vkT_ v) { \
    return castEnum(v); \
}

#define VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(primT_, vkomT_) \
inline primT_ castEnum(vkomT_ v) { \
    return static_cast<primT_>(static_cast<std::underlying_type_t<vkomT_>>(v)); \
} \
inline vkomT_ castEnum(primT_ v) { \
    return static_cast<vkomT_>(v); \
} \
inline operator primT_(vkomT_ v) { \
    return castEnum(v); \
} \
inline operator vkomT_(primT_ v) { \
    return castEnum(v); \
}
#else
#define VKOM_INTERNAL_ENUM_DEFINE_CAST(vkT_, vkomT_) \
template<> \
inline vkT_ castEnum<vkT_, vkomT_>(vkomT_ v) { \
    return static_cast<vkT_>(static_cast<std::underlying_type_t<vkomT_>>(v)); \
} \
template<> \
inline vkomT_ castEnum<vkomT_, vkT_>(vkT_ v) { \
    return static_cast<vkomT_>(static_cast<std::underlying_type_t<vkT_>>(v)); \
}

#define VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(primT_, vkomT_) \
template<> \
inline primT_ castEnum<primT_, vkomT_>(vkomT_ v) { \
    return static_cast<primT_>(static_cast<std::underlying_type_t<vkomT_>>(v)); \
} \
template<> \
inline vkomT_ castEnum<vkomT_, primT_>(primT_ v) { \
    return static_cast<vkomT_>(v); \
}
#endif

namespace vkom {

namespace internal {

template<typename ReturnT, typename ArgT>
inline ReturnT castEnum(ArgT v) {
    /* C++ slop to error at compile time */
    ReturnT::unimplemented_cast();
}

VKOM_INTERNAL_ENUM_DEFINE_CAST(VkObjectType, ObjectType)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkResult, Result)

VKOM_INTERNAL_ENUM_DEFINE_CAST(VkDebugUtilsMessageSeverityFlagBitsEXT, DebugMessageSeverityFlags)
VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(VkDebugUtilsMessageTypeFlagsEXT, DebugMessageTypeFlags)

VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(uint32_t, VendorID)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkDriverId, DriverID)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkPhysicalDeviceType, AdapterType)

VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(VkSurfaceTransformFlagsKHR, SurfaceTransformFlags)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkSurfaceTransformFlagBitsKHR, SurfaceTransformFlags)
VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(VkCompositeAlphaFlagsKHR, CompositeAlphaFlags)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkCompositeAlphaFlagBitsKHR, CompositeAlphaFlags)

/* NOTE: vkom::PresentModeFlags is analogous to VkPresentModeKHR, however it uses flags to
*   represent surface support with one value and when creating swapchains, only one flag must
*   be set
*/
template<>
inline VkPresentModeKHR castEnum<VkPresentModeKHR, PresentModeFlags>(PresentModeFlags v) {
    if ((v & PresentModeFlags::Immediate) != PresentModeFlags::None) {
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    } else if ((v & PresentModeFlags::Mailbox) != PresentModeFlags::None) {
        return VK_PRESENT_MODE_MAILBOX_KHR;
    } else if ((v & PresentModeFlags::Fifo) != PresentModeFlags::None) {
        return VK_PRESENT_MODE_FIFO_KHR;
    } else if ((v & PresentModeFlags::FifoRelaxed) != PresentModeFlags::None) {
        return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
    } else if ((v & PresentModeFlags::SharedDemandRefresh) != PresentModeFlags::None) {
        return VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR;
    } else if ((v & PresentModeFlags::SharedContinuousRefresh) != PresentModeFlags::None) {
        return VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR;
    } else if ((v & PresentModeFlags::FifoLatestReady) != PresentModeFlags::None) {
        return VK_PRESENT_MODE_FIFO_LATEST_READY_KHR;
    }

    return static_cast<VkPresentModeKHR>(0);
}

template<>
inline PresentModeFlags castEnum<PresentModeFlags, VkPresentModeKHR>(VkPresentModeKHR v) {
    switch (v) {
        case VK_PRESENT_MODE_IMMEDIATE_KHR:
            return PresentModeFlags::Immediate;
        case VK_PRESENT_MODE_MAILBOX_KHR:
            return PresentModeFlags::Mailbox;
        case VK_PRESENT_MODE_FIFO_KHR:
            return PresentModeFlags::Fifo;
        case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
            return PresentModeFlags::FifoRelaxed;
        case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
            return PresentModeFlags::SharedDemandRefresh;
        case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
            return PresentModeFlags::SharedContinuousRefresh;
        case VK_PRESENT_MODE_FIFO_LATEST_READY_KHR:
            return PresentModeFlags::FifoLatestReady;
        default:
            break;
    }

    return PresentModeFlags::None;
}

/* NOTE: vkom::ColorSpaceFlags is analogous to VkColorSpaceKHR, however it uses flags to
*   represent surface support with one value and when creating swapchains, only one flag must
*   be set
*/
template<>
inline VkColorSpaceKHR castEnum<VkColorSpaceKHR, ColorSpaceFlags>(ColorSpaceFlags v) {
    if ((v & ColorSpaceFlags::SRGBNonlinear) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    } else if ((v & ColorSpaceFlags::DisplayP3Nonlinear) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT;
    } else if ((v & ColorSpaceFlags::ExtendedSRGBLinear) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT;
    } else if ((v & ColorSpaceFlags::DisplayP3Linear) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT;
    } else if ((v & ColorSpaceFlags::DCIP3Nonlinear) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT;
    } else if ((v & ColorSpaceFlags::BT709Linear) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_BT709_LINEAR_EXT;
    } else if ((v & ColorSpaceFlags::BT709Nonlinear) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_BT709_NONLINEAR_EXT;
    } else if ((v & ColorSpaceFlags::BT2020Linear) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_BT2020_LINEAR_EXT;
    } else if ((v & ColorSpaceFlags::HDR10ST2084) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_HDR10_ST2084_EXT;
    } else if ((v & ColorSpaceFlags::DolbyVision) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_DOLBYVISION_EXT;
    } else if ((v & ColorSpaceFlags::HDR10HLG) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_HDR10_HLG_EXT;
    } else if ((v & ColorSpaceFlags::AdobeRGBLinear) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT;
    } else if ((v & ColorSpaceFlags::AdobeRGBNonlinear) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT;
    } else if ((v & ColorSpaceFlags::PassThrough) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_PASS_THROUGH_EXT;
    } else if ((v & ColorSpaceFlags::ExtendedSRGBNonliner) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT;
    } else if ((v & ColorSpaceFlags::DisplayNative) != ColorSpaceFlags::None) {
        return VK_COLOR_SPACE_DISPLAY_NATIVE_AMD;
    }

    return static_cast<VkColorSpaceKHR>(0);
}

template<>
inline ColorSpaceFlags castEnum<ColorSpaceFlags, VkColorSpaceKHR>(VkColorSpaceKHR v) {
    switch (v) {
        case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
            return ColorSpaceFlags::SRGBNonlinear;
        case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT:
            return ColorSpaceFlags::DisplayP3Nonlinear;
        case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:
            return ColorSpaceFlags::ExtendedSRGBLinear;
        case VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT:
            return ColorSpaceFlags::DisplayP3Linear;
        case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT:
            return ColorSpaceFlags::DCIP3Nonlinear;
        case VK_COLOR_SPACE_BT709_LINEAR_EXT:
            return ColorSpaceFlags::BT709Linear;
        case VK_COLOR_SPACE_BT709_NONLINEAR_EXT:
            return ColorSpaceFlags::BT709Nonlinear;
        case VK_COLOR_SPACE_BT2020_LINEAR_EXT:
            return ColorSpaceFlags::BT2020Linear;
        case VK_COLOR_SPACE_HDR10_ST2084_EXT:
            return ColorSpaceFlags::HDR10ST2084;
        case VK_COLOR_SPACE_DOLBYVISION_EXT:
            return ColorSpaceFlags::DolbyVision;
        case VK_COLOR_SPACE_HDR10_HLG_EXT:
            return ColorSpaceFlags::HDR10HLG;
        case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT:
            return ColorSpaceFlags::AdobeRGBLinear;
        case VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT:
            return ColorSpaceFlags::AdobeRGBNonlinear;
        case VK_COLOR_SPACE_PASS_THROUGH_EXT:
            return ColorSpaceFlags::PassThrough;
        case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT:
            return ColorSpaceFlags::ExtendedSRGBNonliner;
        case VK_COLOR_SPACE_DISPLAY_NATIVE_AMD:
            return ColorSpaceFlags::DisplayNative;
        default:
            break;
    }

    return ColorSpaceFlags::None;
}

/* NOTE: due to vkom::QueueFlags including Present, which is detectible via physical device
*   and surface API rather than queue properties, the two bitflags don't match 1:1 so a
*   manually specified cast is implemented :/
*/
template<>
inline VkQueueFlags castEnum<VkQueueFlags, QueueFlags>(QueueFlags v) {
    return static_cast<VkQueueFlags>(static_cast<std::underlying_type_t<QueueFlags>>(v) & ~static_cast<std::underlying_type_t<QueueFlags>>(QueueFlags::Present));
}

template<>
inline QueueFlags castEnum<QueueFlags, VkQueueFlags>(VkQueueFlags v) {
    return static_cast<QueueFlags>(v);
}

VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(VkPipelineStageFlags, PipelineStageFlags)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkPipelineStageFlagBits, PipelineStageFlags)

VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(VkAccessFlags, ResourceAccessFlags)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkAccessFlagBits, ResourceAccessFlags)

VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(VkBufferUsageFlags, BufferUsageFlags)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkBufferUsageFlagBits, BufferUsageFlags)

VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(VkImageUsageFlags, TextureUsageFlags)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkImageUsageFlagBits, TextureUsageFlags)

VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(VkImageAspectFlags, TextureAspectFlags)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkImageAspectFlagBits, TextureAspectFlags)

VKOM_INTERNAL_ENUM_DEFINE_CAST(VkImageLayout, TextureLayout)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkFormat, Format)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkComponentSwizzle, TextureChannelSwizzle)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkImageViewType, TextureViewType)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkFilter, TexelFilter)

VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(VkShaderStageFlags, ShaderStageFlags)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkShaderStageFlagBits, ShaderStageFlags)


/* NOTE: due to vkom::DescriptorFlags being comprised of flags and not distinct
 *  values, it needs to be parsed and validated to be converted to VkDescriptorType
*/
template<>
inline VkDescriptorType castEnum<VkDescriptorType, DescriptorFlags>(DescriptorFlags v) {
    switch (v) {
        case DescriptorFlags::Sampler:
            return VK_DESCRIPTOR_TYPE_SAMPLER;
        case DescriptorFlags::CombinedTextureSampler:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case DescriptorFlags::SampledTexture:
            return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case DescriptorFlags::StorageTexture:
            return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        case DescriptorFlags::UniformBuffer:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case DescriptorFlags::StorageBuffer:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case DescriptorFlags::UniformTexelBuffer:
            return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
        case DescriptorFlags::StorageTexelBuffer:
            return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        case DescriptorFlags::InputTarget:
            return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        default:
            break;
    }

    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

template<>
inline DescriptorFlags castEnum<DescriptorFlags, VkDescriptorType>(VkDescriptorType v) {
    switch (v) {
        case VK_DESCRIPTOR_TYPE_SAMPLER:
            return DescriptorFlags::Sampler;
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            return DescriptorFlags::CombinedTextureSampler;
        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            return DescriptorFlags::SampledTexture;
        case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            return DescriptorFlags::StorageTexture;
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            return DescriptorFlags::UniformBuffer;
        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            return DescriptorFlags::StorageBuffer;
        case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            return DescriptorFlags::UniformTexelBuffer;
        case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
            return DescriptorFlags::StorageTexelBuffer;
        case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
            return DescriptorFlags::InputTarget;
        default:
            break;
    }

    return DescriptorFlags::None;
}

VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(VkDescriptorSetLayoutCreateFlags, DescriptorSetLayoutFlags)
VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(VkDescriptorPoolCreateFlags, DescriptorPoolFlags)
VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(VkPipelineShaderStageCreateFlags, PipelineShaderFlags)
VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(VkPipelineCreateFlags, PipelineFlags)

VKOM_INTERNAL_ENUM_DEFINE_CAST(VkIndexType, IndexType)

VKOM_INTERNAL_ENUM_DEFINE_CAST_NON_UNDERLYING(VkStencilFaceFlags, StencilFaceFlags)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkStencilFaceFlagBits, StencilFaceFlags)

template<typename Flags>
inline Flags lowestFlag(Flags f) {
    std::underlying_type_t<Flags> u = static_cast<std::underlying_type_t<Flags>>(f);
    for (std::underlying_type_t<Flags> i = 0; i < std::numeric_limits<std::underlying_type_t<Flags>>::digits; i += 1) {
        if ((u & (static_cast<std::underlying_type_t<Flags>>(1) << i)) != static_cast<std::underlying_type_t<Flags>>(0)) {
            return static_cast<Flags>(static_cast<std::underlying_type_t<Flags>>(1) << i);
        }
    }

    return f;
}

}

}
