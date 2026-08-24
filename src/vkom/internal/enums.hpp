#pragma once

#include <limits>
#include <type_traits>

#include <vkom/enums.hpp>

#include <vkom/internal/format.hpp>

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

namespace fmt {

template<>
inline std::string value(Result const& result) {
    switch (result) {
        case Result::Success:
            return "Result::Success";
        case Result::NotReady:
            return "Result::NotReady";
        case Result::Timeout:
            return "Result::Timeout";
        case Result::EventSet:
            return "Result::EventSet";
        case Result::EventReset:
            return "Result::EventReset";
        case Result::Incomplete:
            return "Result::Incomplete";
        case Result::ErrorOutOfHostMemory:
            return "Result::ErrorOutOfHostMemory";
        case Result::ErrorOutOfDeviceMemory:
            return "Result::ErrorOutOfDeviceMemory";
        case Result::ErrorInitializationFailed:
            return "Result::ErrorInitializationFailed";
        case Result::ErrorDeviceLost:
            return "Result::ErrorDeviceLost";
        case Result::ErrorMemoryMapFailed:
            return "Result::ErrorMemoryMapFailed";
        case Result::ErrorLayerNotPresent:
            return "Result::ErrorLayerNotPresent";
        case Result::ErrorExtensionNotPresent:
            return "Result::ErrorExtensionNotPresent";
        case Result::ErrorFeatureNotPresent:
            return "Result::ErrorFeatureNotPresent";
        case Result::ErrorIncompatibleDriver:
            return "Result::ErrorIncompatibleDriver";
        case Result::ErrorTooManyObjects:
            return "Result::ErrorTooManyObjects";
        case Result::ErrorFormatNotSupported:
            return "Result::ErrorFormatNotSupported";
        case Result::ErrorFragmentedPool:
            return "Result::ErrorFragmentedPool";
        case Result::ErrorUnknown:
            return "Result::ErrorUnknown";
        case Result::ErrorUnsupportedFeature:
            return "Result::ErrorUnsupportedFeature";
        case Result::SuboptimalSwapchain:
            return "Result::SuboptimalSwapchain";
        case Result::ErrorSurfaceLost:
            return "Result::ErrorSurfaceLost";
        case Result::ErrorOutOfDateSwapchain:
            return "Result::ErrorOutOfDateSwapchain";
        case Result::ErrorValidationFailed:
            return "Result::ErrorValidationFailed";
        default:
            break;
    }

    return fmt::string("Result::[{}]", static_cast<std::underlying_type_t<Result>>(result));
}

template<>
inline std::string value(ObjectType const& type) {
    switch (type) {
        case ObjectType::Unknown:
            return "ObjectType::Unknown";
        case ObjectType::Instance:
            return "ObjectType::Instance";
        case ObjectType::PhysicalDevice:
            return "ObjectType::PhysicalDevice";
        case ObjectType::Device:
            return "ObjectType::Device";
        case ObjectType::Queue:
            return "ObjectType::Queue";
        case ObjectType::Semaphore:
            return "ObjectType::Semaphore";
        case ObjectType::CommandBuffer:
            return "ObjectType::CommandBuffer";
        case ObjectType::Fence:
            return "ObjectType::Fence";
        case ObjectType::DeviceMemory:
            return "ObjectType::DeviceMemory";
        case ObjectType::Buffer:
            return "ObjectType::Buffer";
        case ObjectType::Image:
            return "ObjectType::Image";
        case ObjectType::Event:
            return "ObjectType::Event";
        case ObjectType::QueryPool:
            return "ObjectType::QueryPool";
        case ObjectType::BufferView:
            return "ObjectType::BufferView";
        case ObjectType::ImageView:
            return "ObjectType::ImageView";
        case ObjectType::ShaderModule:
            return "ObjectType::ShaderModule";
        case ObjectType::PipelineCache:
            return "ObjectType::PipelineCache";
        case ObjectType::PipelineLayout:
            return "ObjectType::PipelineLayout";
        case ObjectType::RenderPass:
            return "ObjectType::RenderPass";
        case ObjectType::Pipeline:
            return "ObjectType::Pipeline";
        case ObjectType::DescriptorSetLayout:
            return "ObjectType::DescriptorSetLayout";
        case ObjectType::Sampler:
            return "ObjectType::Sampler";
        case ObjectType::DescriptorPool:
            return "ObjectType::DescriptorPool";
        case ObjectType::DescriptorSet:
            return "ObjectType::DescriptorSet";
        case ObjectType::Framebuffer:
            return "ObjectType::Framebuffer";
        case ObjectType::CommandPool:
            return "ObjectType::CommandPool";
        case ObjectType::SurfaceKHR:
            return "ObjectType::SurfaceKHR";
        case ObjectType::SwapchainKHR:
            return "ObjectType::SwapchainKHR";
        case ObjectType::DisplayKHR:
            return "ObjectType::DisplayKHR";
        case ObjectType::DisplayModeKHR:
            return "ObjectType::DisplayModeKHR";
        case ObjectType::DebugUtilsMessengerEXT:
            return "ObjectType::DebugUtilsMessengerEXT";
        case ObjectType::VmaPool:
            return "ObjectType::VmaPool";
        default:
            break;
    }

    return fmt::string("ObjectType::[{}]", static_cast<std::underlying_type_t<ObjectType>>(type));
}

template<>
inline std::string value(DebugMessageSeverityFlags const& flags) {
    std::stringstream ss = {};
    ss << "DebugMessageSeverityFlags::[";

    if ((flags & DebugMessageSeverityFlags::Verbose) != DebugMessageSeverityFlags::None) {
        ss << "Verbose,";
    }

    if ((flags & DebugMessageSeverityFlags::Info) != DebugMessageSeverityFlags::None) {
        ss << "Info,";
    }

    if ((flags & DebugMessageSeverityFlags::Warning) != DebugMessageSeverityFlags::None) {
        ss << "Warning,";
    }

    if ((flags & DebugMessageSeverityFlags::Error) != DebugMessageSeverityFlags::None) {
        ss << "Error,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(DebugMessageTypeFlags const& flags) {
    std::stringstream ss = {};
    ss << "DebugMessageTypeFlags::[";

    if ((flags & DebugMessageTypeFlags::General) != DebugMessageTypeFlags::None) {
        ss << "General,";
    }

    if ((flags & DebugMessageTypeFlags::Validation) != DebugMessageTypeFlags::None) {
        ss << "Validation,";
    }

    if ((flags & DebugMessageTypeFlags::Performance) != DebugMessageTypeFlags::None) {
        ss << "Performance,";
    }

    if ((flags & DebugMessageTypeFlags::DeviceAddressBinding) != DebugMessageTypeFlags::None) {
        ss << "DeviceAddressBinding,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(SurfaceWSIType const& type) {
    switch (type) {
        case SurfaceWSIType::Win32:
            return "SurfaceWSIType::Win32";
        case SurfaceWSIType::Xlib:
            return "SurfaceWSIType::Xlib";
        case SurfaceWSIType::Xcb:
            return "SurfaceWSIType::Xcb";
        case SurfaceWSIType::Wayland:
            return "SurfaceWSIType::Wayland";
        case SurfaceWSIType::Metal:
            return "SurfaceWSIType::Metal";
        case SurfaceWSIType::Cocoa:
            return "SurfaceWSIType::Cocoa";
        default:
            break;
    }

    return fmt::string("SurfaceWSIType::[{}]", static_cast<std::underlying_type_t<SurfaceWSIType>>(type));
}

template<>
inline std::string value(VendorID const& id) {
    switch (id) {
        case VendorID::Unknown:
            return "VendorID::Unknown";
        case VendorID::AMD:
            return "VendorID::AMD";
        case VendorID::ImgTec:
            return "VendorID::ImgTec";
        case VendorID::Apple:
            return "VendorID::Apple";
        case VendorID::Nvidia:
            return "VendorID::Nvidia";
        case VendorID::ARM:
            return "VendorID::ARM";
        case VendorID::Broadcom:
            return "VendorID::Broadcom";
        case VendorID::Vivante:
            return "VendorID::Vivante";
        case VendorID::Qualcomm:
            return "VendorID::Qualcomm";
        case VendorID::Intel:
            return "VendorID::Intel";
        case VendorID::Mesa:
            return "VendorID::Mesa";
        default:
            break;
    }

    return fmt::string("VendorID::[{}]", static_cast<std::underlying_type_t<VendorID>>(id));
}

template<>
inline std::string value(DriverID const& id) {
    switch (id) {
        case DriverID::Unknown:
            return "DriverID::Unknown";
        case DriverID::AMDProprietary:
            return "DriverID::AMDProprietary";
        case DriverID::AMDOpenSource:
            return "DriverID::AMDOpenSource";
        case DriverID::MesaRADV:
            return "DriverID::MesaRADV";
        case DriverID::NvidiaProprietary:
            return "DriverID::NvidiaProprietary";
        case DriverID::IntelProprietaryWindows:
            return "DriverID::IntelProprietaryWindows";
        case DriverID::IntelOpenSourceMesa:
            return "DriverID::IntelOpenSourceMesa";
        case DriverID::ImaginationProprietary:
            return "DriverID::ImaginationProprietary";
        case DriverID::QualcommProprietary:
            return "DriverID::QualcommProprietary";
        case DriverID::ARMProprietary:
            return "DriverID::ARMProprietary";
        case DriverID::GoogleSwiftShader:
            return "DriverID::GoogleSwiftShader";
        case DriverID::GGPProprietary:
            return "DriverID::GGPProprietary";
        case DriverID::BroadcomProprietary:
            return "DriverID::BroadcomProprietary";
        case DriverID::MesaLLVMPipe:
            return "DriverID::MesaLLVMPipe";
        case DriverID::MoltenVK:
            return "DriverID::MoltenVK";
        case DriverID::CoreAVIProprietary:
            return "DriverID::CoreAVIProprietary";
        case DriverID::JuiceProprietary:
            return "DriverID::JuiceProprietary";
        case DriverID::VerisiliconProprietary:
            return "DriverID::VerisiliconProprietary";
        case DriverID::MesaTurnip:
            return "DriverID::MesaTurnip";
        case DriverID::MesaV3DV:
            return "DriverID::MesaV3DV";
        case DriverID::MesaPanVK:
            return "DriverID::MesaPanVK";
        case DriverID::SamsungProprietary:
            return "DriverID::SamsungProprietary";
        case DriverID::MesaVenus:
            return "DriverID::MesaVenus";
        case DriverID::MesaDozen:
            return "DriverID::MesaDozen";
        case DriverID::MesaNVK:
            return "DriverID::MesaNVK";
        case DriverID::ImaginationOpenSourceMesa:
            return "DriverID::ImaginationOpenSourceMesa";
        case DriverID::MesaHoneyKrisp:
            return "DriverID::MesaHoneyKrisp";
        case DriverID::VulkanSCEmulationOnVulkan:
            return "DriverID::VulkanSCEmulationOnVulkan";
        case DriverID::MesaKosmicKrisp:
            return "DriverID::MesaKosmicKrisp";
        default:
            break;
    }

    return fmt::string("DriverID::[{}]", static_cast<std::underlying_type_t<DriverID>>(id));
}

template<>
inline std::string value(AdapterType const& type) {
    switch (type) {
        case AdapterType::Other:
            return "AdapterType::Other";
        case AdapterType::Integrated:
            return "AdapterType::Integrated";
        case AdapterType::Dedicated:
            return "AdapterType::Dedicated";
        case AdapterType::Virtual:
            return "AdapterType::Virtual";
        case AdapterType::CPU:
            return "AdapterType::CPU";
        default:
            break;
    }

    return fmt::string("AdapterType::[{}]", static_cast<std::underlying_type_t<AdapterType>>(type));
}

template<>
inline std::string value(SurfaceTransformFlags const& flags) {
    std::stringstream ss = {};
    ss << "SurfaceTransformFlags::[";

    if ((flags & SurfaceTransformFlags::Identity) != SurfaceTransformFlags::None) {
        ss << "Identity,";
    }

    if ((flags & SurfaceTransformFlags::Rotate90) != SurfaceTransformFlags::None) {
        ss << "Rotate90,";
    }

    if ((flags & SurfaceTransformFlags::Rotate180) != SurfaceTransformFlags::None) {
        ss << "Rotate180,";
    }

    if ((flags & SurfaceTransformFlags::Rotate270) != SurfaceTransformFlags::None) {
        ss << "Rotate270,";
    }

    if ((flags & SurfaceTransformFlags::HorizontalMirror) != SurfaceTransformFlags::None) {
        ss << "HorizontalMirror,";
    }

    if ((flags & SurfaceTransformFlags::HorizontalMirrorRotate90) != SurfaceTransformFlags::None) {
        ss << "HorizontalMirrorRotate90,";
    }

    if ((flags & SurfaceTransformFlags::HorizontalMirrorRotate180) != SurfaceTransformFlags::None) {
        ss << "HorizontalMirrorRotate180,";
    }

    if ((flags & SurfaceTransformFlags::HorizontalMirrorRotate270) != SurfaceTransformFlags::None) {
        ss << "HorizontalMirrorRotate270,";
    }

    if ((flags & SurfaceTransformFlags::Inherit) != SurfaceTransformFlags::None) {
        ss << "Inherit,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(CompositeAlphaFlags const& flags) {
    std::stringstream ss = {};
    ss << "CompositeAlphaFlags::[";

    if ((flags & CompositeAlphaFlags::Opaque) != CompositeAlphaFlags::None) {
        ss << "Opaque,";
    }

    if ((flags & CompositeAlphaFlags::PreMultiplied) != CompositeAlphaFlags::None) {
        ss << "PreMultiplied,";
    }

    if ((flags & CompositeAlphaFlags::PostMultiplied) != CompositeAlphaFlags::None) {
        ss << "PostMultiplied,";
    }

    if ((flags & CompositeAlphaFlags::Inherit) != CompositeAlphaFlags::None) {
        ss << "Inherit,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(PresentModeFlags const& flags) {
    if (flags == PresentModeFlags::All) {
        return "PresentModeFlags::[All]";
    }

    std::stringstream ss = {};
    ss << "PresentModeFlags::[";

    if ((flags & PresentModeFlags::Immediate) != PresentModeFlags::None) {
        ss << "Immediate,";
    }

    if ((flags & PresentModeFlags::Mailbox) != PresentModeFlags::None) {
        ss << "Mailbox,";
    }

    if ((flags & PresentModeFlags::Fifo) != PresentModeFlags::None) {
        ss << "Fifo,";
    }

    if ((flags & PresentModeFlags::FifoRelaxed) != PresentModeFlags::None) {
        ss << "FifoRelaxed,";
    }

    if ((flags & PresentModeFlags::SharedDemandRefresh) != PresentModeFlags::None) {
        ss << "SharedDemandRefresh,";
    }

    if ((flags & PresentModeFlags::SharedContinuousRefresh) != PresentModeFlags::None) {
        ss << "SharedContinuousRefresh,";
    }

    if ((flags & PresentModeFlags::FifoLatestReady) != PresentModeFlags::None) {
        ss << "FifoLatestReady,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(ColorSpaceFlags const& flags) {
    if (flags == ColorSpaceFlags::All) {
        return "ColorSpaceFlags::[All]";
    }

    std::stringstream ss = {};
    ss << "ColorSpaceFlags::[";

    if ((flags & ColorSpaceFlags::SRGBNonlinear) != ColorSpaceFlags::None) {
        ss << "SRGBNonlinear,";
    }

    if ((flags & ColorSpaceFlags::DisplayP3Nonlinear) != ColorSpaceFlags::None) {
        ss << "DisplayP3Nonlinear,";
    }

    if ((flags & ColorSpaceFlags::ExtendedSRGBLinear) != ColorSpaceFlags::None) {
        ss << "ExtendedSRGBLinear,";
    }

    if ((flags & ColorSpaceFlags::DisplayP3Linear) != ColorSpaceFlags::None) {
        ss << "DisplayP3Linear,";
    }

    if ((flags & ColorSpaceFlags::DCIP3Nonlinear) != ColorSpaceFlags::None) {
        ss << "DCIP3Nonlinear,";
    }

    if ((flags & ColorSpaceFlags::BT709Linear) != ColorSpaceFlags::None) {
        ss << "BT709Linear,";
    }

    if ((flags & ColorSpaceFlags::BT709Nonlinear) != ColorSpaceFlags::None) {
        ss << "BT709Nonlinear,";
    }

    if ((flags & ColorSpaceFlags::BT2020Linear) != ColorSpaceFlags::None) {
        ss << "BT2020Linear,";
    }

    if ((flags & ColorSpaceFlags::HDR10ST2084) != ColorSpaceFlags::None) {
        ss << "HDR10ST2084,";
    }

    if ((flags & ColorSpaceFlags::DolbyVision) != ColorSpaceFlags::None) {
        ss << "DolbyVision,";
    }

    if ((flags & ColorSpaceFlags::HDR10HLG) != ColorSpaceFlags::None) {
        ss << "HDR10HLG,";
    }

    if ((flags & ColorSpaceFlags::AdobeRGBLinear) != ColorSpaceFlags::None) {
        ss << "AdobeRGBLinear,";
    }

    if ((flags & ColorSpaceFlags::AdobeRGBNonlinear) != ColorSpaceFlags::None) {
        ss << "AdobeRGBNonlinear,";
    }

    if ((flags & ColorSpaceFlags::PassThrough) != ColorSpaceFlags::None) {
        ss << "PassThrough,";
    }

    if ((flags & ColorSpaceFlags::ExtendedSRGBNonliner) != ColorSpaceFlags::None) {
        ss << "ExtendedSRGBNonliner,";
    }

    if ((flags & ColorSpaceFlags::DisplayNative) != ColorSpaceFlags::None) {
        ss << "DisplayNative,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(QueueFlags const& flags) {
    std::stringstream ss = {};
    ss << "QueueFlags::[";

    if ((flags & QueueFlags::Graphics) != QueueFlags::None) {
        ss << "Graphics,";
    }

    if ((flags & QueueFlags::Transfer) != QueueFlags::None) {
        ss << "Transfer,";
    }

    if ((flags & QueueFlags::Compute) != QueueFlags::None) {
        ss << "Compute,";
    }

    if ((flags & QueueFlags::SparseBinding) != QueueFlags::None) {
        ss << "SparseBinding,";
    }

    if ((flags & QueueFlags::Protected) != QueueFlags::None) {
        ss << "Protected,";
    }

    if ((flags & QueueFlags::VideoDecode) != QueueFlags::None) {
        ss << "VideoDecode,";
    }

    if ((flags & QueueFlags::VideoEncode) != QueueFlags::None) {
        ss << "VideoEncode,";
    }

    if ((flags & QueueFlags::Present) != QueueFlags::None) {
        ss << "Present,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(MemoryLocationFlags const& flags) {
    std::stringstream ss = {};
    ss << "MemoryLocationFlags::[";

    if ((flags & MemoryLocationFlags::GPU) != MemoryLocationFlags::None) {
        ss << "GPU,";
    }

    if ((flags & MemoryLocationFlags::CPU) != MemoryLocationFlags::None) {
        ss << "CPU,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(PipelineStageFlags const& flags) {
    std::stringstream ss = {};
    ss << "PipelineStageFlags::[";

    if ((flags & PipelineStageFlags::TopOfPipe) != PipelineStageFlags::None) {
        ss << "TopOfPipe,";
    }

    if ((flags & PipelineStageFlags::DrawIndirect) != PipelineStageFlags::None) {
        ss << "DrawIndirect,";
    }

    if ((flags & PipelineStageFlags::VertexInput) != PipelineStageFlags::None) {
        ss << "VertexInput,";
    }

    if ((flags & PipelineStageFlags::VertexShader) != PipelineStageFlags::None) {
        ss << "VertexShader,";
    }

    if ((flags & PipelineStageFlags::HullShader) != PipelineStageFlags::None) {
        ss << "HullShader,";
    }

    if ((flags & PipelineStageFlags::DomainShader) != PipelineStageFlags::None) {
        ss << "DomainShader,";
    }

    if ((flags & PipelineStageFlags::GeometryShader) != PipelineStageFlags::None) {
        ss << "GeometryShader,";
    }

    if ((flags & PipelineStageFlags::FragmentShader) != PipelineStageFlags::None) {
        ss << "FragmentShader,";
    }

    if ((flags & PipelineStageFlags::EarlyFragmentTests) != PipelineStageFlags::None) {
        ss << "EarlyFragmentTests,";
    }

    if ((flags & PipelineStageFlags::LateFragmentTests) != PipelineStageFlags::None) {
        ss << "LateFragmentTests,";
    }

    if ((flags & PipelineStageFlags::RenderTarget) != PipelineStageFlags::None) {
        ss << "RenderTarget,";
    }

    if ((flags & PipelineStageFlags::ComputeShader) != PipelineStageFlags::None) {
        ss << "ComputeShader,";
    }

    if ((flags & PipelineStageFlags::Transfer) != PipelineStageFlags::None) {
        ss << "Transfer,";
    }

    if ((flags & PipelineStageFlags::BottomOfPipe) != PipelineStageFlags::None) {
        ss << "BottomOfPipe,";
    }

    if ((flags & PipelineStageFlags::Host) != PipelineStageFlags::None) {
        ss << "Host,";
    }

    if ((flags & PipelineStageFlags::AllGraphics) != PipelineStageFlags::None) {
        ss << "AllGraphics,";
    }

    if ((flags & PipelineStageFlags::AllCommands) != PipelineStageFlags::None) {
        ss << "AllCommands,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(ResourceAccessFlags const& flags) {
    std::stringstream ss = {};
    ss << "ResourceAccessFlags::[";

    if ((flags & ResourceAccessFlags::IndirectCommandRead) != ResourceAccessFlags::None) {
        ss << "IndirectCommandRead,";
    }

    if ((flags & ResourceAccessFlags::IndexRead) != ResourceAccessFlags::None) {
        ss << "IndexRead,";
    }

    if ((flags & ResourceAccessFlags::VertexAttributeRead) != ResourceAccessFlags::None) {
        ss << "VertexAttributeRead,";
    }

    if ((flags & ResourceAccessFlags::UniformRead) != ResourceAccessFlags::None) {
        ss << "UniformRead,";
    }

    if ((flags & ResourceAccessFlags::InputAttachmentRead) != ResourceAccessFlags::None) {
        ss << "InputAttachmentRead,";
    }

    if ((flags & ResourceAccessFlags::ShaderRead) != ResourceAccessFlags::None) {
        ss << "ShaderRead,";
    }

    if ((flags & ResourceAccessFlags::ShaderWrite) != ResourceAccessFlags::None) {
        ss << "ShaderWrite,";
    }

    if ((flags & ResourceAccessFlags::RenderTargetRead) != ResourceAccessFlags::None) {
        ss << "RenderTargetRead,";
    }

    if ((flags & ResourceAccessFlags::RenderTargetWrite) != ResourceAccessFlags::None) {
        ss << "RenderTargetWrite,";
    }

    if ((flags & ResourceAccessFlags::DepthStencilTargetRead) != ResourceAccessFlags::None) {
        ss << "DepthStencilTargetRead,";
    }

    if ((flags & ResourceAccessFlags::DepthStencilTargetWrite) != ResourceAccessFlags::None) {
        ss << "DepthStencilTargetWrite,";
    }

    if ((flags & ResourceAccessFlags::TransferRead) != ResourceAccessFlags::None) {
        ss << "TransferRead,";
    }

    if ((flags & ResourceAccessFlags::TransferWrite) != ResourceAccessFlags::None) {
        ss << "TransferWrite,";
    }

    if ((flags & ResourceAccessFlags::HostRead) != ResourceAccessFlags::None) {
        ss << "HostRead,";
    }

    if ((flags & ResourceAccessFlags::HostWrite) != ResourceAccessFlags::None) {
        ss << "HostWrite,";
    }

    if ((flags & ResourceAccessFlags::MemoryRead) != ResourceAccessFlags::None) {
        ss << "MemoryRead,";
    }

    if ((flags & ResourceAccessFlags::MemoryWrite) != ResourceAccessFlags::None) {
        ss << "MemoryWrite,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(BufferUsageFlags const& flags) {
    std::stringstream ss = {};
    ss << "BufferUsageFlags::[";

    if ((flags & BufferUsageFlags::TransferSource) != BufferUsageFlags::None) {
        ss << "TransferSource,";
    }

    if ((flags & BufferUsageFlags::TransferDestination) != BufferUsageFlags::None) {
        ss << "TransferDestination,";
    }

    if ((flags & BufferUsageFlags::UniformTexelBuffer) != BufferUsageFlags::None) {
        ss << "UniformTexelBuffer,";
    }

    if ((flags & BufferUsageFlags::StorageTexelBuffer) != BufferUsageFlags::None) {
        ss << "StorageTexelBuffer,";
    }

    if ((flags & BufferUsageFlags::UniformBuffer) != BufferUsageFlags::None) {
        ss << "UniformBuffer,";
    }

    if ((flags & BufferUsageFlags::StorageBuffer) != BufferUsageFlags::None) {
        ss << "StorageBuffer,";
    }

    if ((flags & BufferUsageFlags::IndexBuffer) != BufferUsageFlags::None) {
        ss << "IndexBuffer,";
    }

    if ((flags & BufferUsageFlags::VertexBuffer) != BufferUsageFlags::None) {
        ss << "VertexBuffer,";
    }

    if ((flags & BufferUsageFlags::IndirectBuffer) != BufferUsageFlags::None) {
        ss << "IndirectBuffer,";
    }

    if ((flags & BufferUsageFlags::VideoDecodeSource) != BufferUsageFlags::None) {
        ss << "VideoDecodeSource,";
    }

    if ((flags & BufferUsageFlags::VideoDecodeDestination) != BufferUsageFlags::None) {
        ss << "VideoDecodeDestination,";
    }

    if ((flags & BufferUsageFlags::VideoEncodeSource) != BufferUsageFlags::None) {
        ss << "VideoEncodeSource,";
    }

    if ((flags & BufferUsageFlags::VideoEncodeDestination) != BufferUsageFlags::None) {
        ss << "VideoEncodeDestination,";
    }

    if ((flags & BufferUsageFlags::ShaderDeviceAddress) != BufferUsageFlags::None) {
        ss << "ShaderDeviceAddress,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(TextureUsageFlags const& flags) {
    std::stringstream ss = {};
    ss << "TextureUsageFlags::[";

    if ((flags & TextureUsageFlags::TransferSource) != TextureUsageFlags::None) {
        ss << "TransferSource,";
    }

    if ((flags & TextureUsageFlags::TransferDestination) != TextureUsageFlags::None) {
        ss << "TransferDestination,";
    }

    if ((flags & TextureUsageFlags::Sampled) != TextureUsageFlags::None) {
        ss << "Sampled,";
    }

    if ((flags & TextureUsageFlags::Storage) != TextureUsageFlags::None) {
        ss << "Storage,";
    }

    if ((flags & TextureUsageFlags::RenderTarget) != TextureUsageFlags::None) {
        ss << "RenderTarget,";
    }

    if ((flags & TextureUsageFlags::DepthStencilTarget) != TextureUsageFlags::None) {
        ss << "DepthStencilTarget,";
    }

    if ((flags & TextureUsageFlags::TransientTarget) != TextureUsageFlags::None) {
        ss << "TransientTarget,";
    }

    if ((flags & TextureUsageFlags::InputTarget) != TextureUsageFlags::None) {
        ss << "InputTarget,";
    }

    if ((flags & TextureUsageFlags::HostTransfer) != TextureUsageFlags::None) {
        ss << "HostTransfer,";
    }

    if ((flags & TextureUsageFlags::VideoDecodeDestination) != TextureUsageFlags::None) {
        ss << "VideoDecodeDestination,";
    }

    if ((flags & TextureUsageFlags::VideoDecodeSource) != TextureUsageFlags::None) {
        ss << "VideoDecodeSource,";
    }

    if ((flags & TextureUsageFlags::VideoDecodeDPB) != TextureUsageFlags::None) {
        ss << "VideoDecodeDPB,";
    }

    if ((flags & TextureUsageFlags::VideoEncodeDestination) != TextureUsageFlags::None) {
        ss << "VideoEncodeDestination,";
    }

    if ((flags & TextureUsageFlags::VideoEncodeSource) != TextureUsageFlags::None) {
        ss << "VideoEncodeSource,";
    }

    if ((flags & TextureUsageFlags::VideoEncodeDPB) != TextureUsageFlags::None) {
        ss << "VideoEncodeDPB,";
    }

    if ((flags & TextureUsageFlags::FragmentShadingRateTarget) != TextureUsageFlags::None) {
        ss << "FragmentShadingRateTarget,";
    }

    if ((flags & TextureUsageFlags::FragmentDensityMap) != TextureUsageFlags::None) {
        ss << "FragmentDensityMap,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(TextureLayout const& type) {
    switch (type) {
        case TextureLayout::Undefined:
            return "TextureLayout::Undefined";
        case TextureLayout::General:
            return "TextureLayout::General";
        case TextureLayout::RenderTarget:
            return "TextureLayout::RenderTarget";
        case TextureLayout::DepthStencilTarget:
            return "TextureLayout::DepthStencilTarget";
        case TextureLayout::DepthStencilReadOnly:
            return "TextureLayout::DepthStencilReadOnly";
        case TextureLayout::ShaderReadOnly:
            return "TextureLayout::ShaderReadOnly";
        case TextureLayout::TransferSource:
            return "TextureLayout::TransferSource";
        case TextureLayout::TransferDestination:
            return "TextureLayout::TransferDestination";
        case TextureLayout::Preinitialized:
            return "TextureLayout::Preinitialized";
        case TextureLayout::PresentSource:
            return "TextureLayout::PresentSource";
        default:
            break;
    }

    return fmt::string("TextureLayout::[{}]", static_cast<std::underlying_type_t<TextureLayout>>(type));
}

template<>
inline std::string value(Format const& type) {
    switch (type) {
        case Format::Undefined:
            return "Format::Undefined";
        case Format::R8UnsignedNorm:
            return "Format::R8UnsignedNorm";
        case Format::R8SignedNorm:
            return "Format::R8SignedNorm";
        case Format::R8UnsignedInt:
            return "Format::R8UnsignedInt";
        case Format::R8SignedInt:
            return "Format::R8SignedInt";
        case Format::R8UnsignedNormSRGB:
            return "Format::R8UnsignedNormSRGB";
        case Format::RG8UnsignedNorm:
            return "Format::RG8UnsignedNorm";
        case Format::RG8SignedNorm:
            return "Format::RG8SignedNorm";
        case Format::RG8UnsignedInt:
            return "Format::RG8UnsignedInt";
        case Format::RG8SignedInt:
            return "Format::RG8SignedInt";
        case Format::RG8UnsignedNormSRGB:
            return "Format::RG8UnsignedNormSRGB";
        case Format::RGB8UnsignedNorm:
            return "Format::RGB8UnsignedNorm";
        case Format::RGB8SignedNorm:
            return "Format::RGB8SignedNorm";
        case Format::RGB8UnsignedInt:
            return "Format::RGB8UnsignedInt";
        case Format::RGB8SignedInt:
            return "Format::RGB8SignedInt";
        case Format::RGB8UnsignedNormSRGB:
            return "Format::RGB8UnsignedNormSRGB";
        case Format::RGBA8UnsignedNorm:
            return "Format::RGBA8UnsignedNorm";
        case Format::RGBA8SignedNorm:
            return "Format::RGBA8SignedNorm";
        case Format::RGBA8UnsignedInt:
            return "Format::RGBA8UnsignedInt";
        case Format::RGBA8SignedInt:
            return "Format::RGBA8SignedInt";
        case Format::RGBA8UnsignedNormSRGB:
            return "Format::RGBA8UnsignedNormSRGB";
        case Format::BGR8UnsignedNorm:
            return "Format::BGR8UnsignedNorm";
        case Format::BGR8SignedNorm:
            return "Format::BGR8SignedNorm";
        case Format::BGR8UnsignedInt:
            return "Format::BGR8UnsignedInt";
        case Format::BGR8SignedInt:
            return "Format::BGR8SignedInt";
        case Format::BGR8UnsignedNormSRGB:
            return "Format::BGR8UnsignedNormSRGB";
        case Format::BGRA8UnsignedNorm:
            return "Format::BGRA8UnsignedNorm";
        case Format::BGRA8SignedNorm:
            return "Format::BGRA8SignedNorm";
        case Format::BGRA8UnsignedInt:
            return "Format::BGRA8UnsignedInt";
        case Format::BGRA8SignedInt:
            return "Format::BGRA8SignedInt";
        case Format::BGRA8UnsignedNormSRGB:
            return "Format::BGRA8UnsignedNormSRGB";
        case Format::R16UnsignedNorm:
            return "Format::R16UnsignedNorm";
        case Format::R16SignedNorm:
            return "Format::R16SignedNorm";
        case Format::R16UnsignedInt:
            return "Format::R16UnsignedInt";
        case Format::R16SignedInt:
            return "Format::R16SignedInt";
        case Format::R16SignedFloat:
            return "Format::R16SignedFloat";
        case Format::RG16UnsignedNorm:
            return "Format::RG16UnsignedNorm";
        case Format::RG16SignedNorm:
            return "Format::RG16SignedNorm";
        case Format::RG16UnsignedInt:
            return "Format::RG16UnsignedInt";
        case Format::RG16SignedInt:
            return "Format::RG16SignedInt";
        case Format::RG16SignedFloat:
            return "Format::RG16SignedFloat";
        case Format::RGB16UnsignedNorm:
            return "Format::RGB16UnsignedNorm";
        case Format::RGB16SignedNorm:
            return "Format::RGB16SignedNorm";
        case Format::RGB16UnsignedInt:
            return "Format::RGB16UnsignedInt";
        case Format::RGB16SignedInt:
            return "Format::RGB16SignedInt";
        case Format::RGB16SignedFloat:
            return "Format::RGB16SignedFloat";
        case Format::RGBA16UnsignedNorm:
            return "Format::RGBA16UnsignedNorm";
        case Format::RGBA16SignedNorm:
            return "Format::RGBA16SignedNorm";
        case Format::RGBA16UnsignedInt:
            return "Format::RGBA16UnsignedInt";
        case Format::RGBA16SignedInt:
            return "Format::RGBA16SignedInt";
        case Format::RGBA16SignedFloat:
            return "Format::RGBA16SignedFloat";
        case Format::R32UnsignedInt:
            return "Format::R32UnsignedInt";
        case Format::R32SignedInt:
            return "Format::R32SignedInt";
        case Format::R32SignedFloat:
            return "Format::R32SignedFloat";
        case Format::RG32UnsignedInt:
            return "Format::RG32UnsignedInt";
        case Format::RG32SignedInt:
            return "Format::RG32SignedInt";
        case Format::RG32SignedFloat:
            return "Format::RG32SignedFloat";
        case Format::RGB32UnsignedInt:
            return "Format::RGB32UnsignedInt";
        case Format::RGB32SignedInt:
            return "Format::RGB32SignedInt";
        case Format::RGB32SignedFloat:
            return "Format::RGB32SignedFloat";
        case Format::RGBA32UnsignedInt:
            return "Format::RGBA32UnsignedInt";
        case Format::RGBA32SignedInt:
            return "Format::RGBA32SignedInt";
        case Format::RGBA32SignedFloat:
            return "Format::RGBA32SignedFloat";
        case Format::R64UnsignedInt:
            return "Format::R64UnsignedInt";
        case Format::R64SignedInt:
            return "Format::R64SignedInt";
        case Format::R64SignedFloat:
            return "Format::R64SignedFloat";
        case Format::RG64UnsignedInt:
            return "Format::RG64UnsignedInt";
        case Format::RG64SignedInt:
            return "Format::RG64SignedInt";
        case Format::RG64SignedFloat:
            return "Format::RG64SignedFloat";
        case Format::RGB64UnsignedInt:
            return "Format::RGB64UnsignedInt";
        case Format::RGB64SignedInt:
            return "Format::RGB64SignedInt";
        case Format::RGB64SignedFloat:
            return "Format::RGB64SignedFloat";
        case Format::RGBA64UnsignedInt:
            return "Format::RGBA64UnsignedInt";
        case Format::RGBA64SignedInt:
            return "Format::RGBA64SignedInt";
        case Format::RGBA64SignedFloat:
            return "Format::RGBA64SignedFloat";
        case Format::B10G11R11UnsignedFloat:
            return "Format::B10G11R11UnsignedFloat";
        case Format::E5B9G9R9UnsignedFloat:
            return "Format::E5B9G9R9UnsignedFloat";
        case Format::Depth16UnsignedNorm:
            return "Format::Depth16UnsignedNorm";
        case Format::Depth32SignedFloat:
            return "Format::Depth32SignedFloat";
        case Format::Stencil8UnsignedInt:
            return "Format::Stencil8UnsignedInt";
        case Format::Depth16UnsignedNormStencil8UnsignedInt:
            return "Format::Depth16UnsignedNormStencil8UnsignedInt";
        case Format::Depth24UnsignedNormStencil8UnsignedInt:
            return "Format::Depth24UnsignedNormStencil8UnsignedInt";
        case Format::Depth32SignedFloatStencil8UnsignedInt:
            return "Format::Depth32SignedFloatStencil8UnsignedInt";
        default:
            break;
    }

    return fmt::string("Format::[{}]", static_cast<std::underlying_type_t<Format>>(type));
}

template<>
inline std::string value(TextureChannelSwizzle const& type) {
    switch (type) {
        case TextureChannelSwizzle::Identity:
            return "TextureChannelSwizzle::Identity";
        case TextureChannelSwizzle::Zero:
            return "TextureChannelSwizzle::Zero";
        case TextureChannelSwizzle::One:
            return "TextureChannelSwizzle::One";
        case TextureChannelSwizzle::R:
            return "TextureChannelSwizzle::R";
        case TextureChannelSwizzle::G:
            return "TextureChannelSwizzle::G";
        case TextureChannelSwizzle::B:
            return "TextureChannelSwizzle::B";
        case TextureChannelSwizzle::A:
            return "TextureChannelSwizzle::A";
        default:
            break;
    }

    return fmt::string("TextureChannelSwizzle::[{}]", static_cast<std::underlying_type_t<TextureChannelSwizzle>>(type));
}

template<>
inline std::string value(TextureAspectFlags const& flags) {
    std::stringstream ss = {};
    ss << "TextureAspectFlags::[";

    if ((flags & TextureAspectFlags::Color) != TextureAspectFlags::None) {
        ss << "Color,";
    }

    if ((flags & TextureAspectFlags::Depth) != TextureAspectFlags::None) {
        ss << "Depth,";
    }

    if ((flags & TextureAspectFlags::Stencil) != TextureAspectFlags::None) {
        ss << "Stencil,";
    }

    if ((flags & TextureAspectFlags::Metadata) != TextureAspectFlags::None) {
        ss << "Metadata,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(TextureViewType const& type) {
    switch (type) {
        case TextureViewType::D1:
            return "TextureViewType::D1";
        case TextureViewType::D2:
            return "TextureViewType::D2";
        case TextureViewType::D3:
            return "TextureViewType::D3";
        case TextureViewType::D1Array:
            return "TextureViewType::D1Array";
        case TextureViewType::D2Array:
            return "TextureViewType::D2Array";
        case TextureViewType::Cube:
            return "TextureViewType::Cube";
        case TextureViewType::CubeArray:
            return "TextureViewType::CubeArray";
        default:
            break;
    }

    return fmt::string("TextureViewType::[{}]", static_cast<std::underlying_type_t<TextureViewType>>(type));
}

template<>
inline std::string value(TexelFilter const& type) {
    switch (type) {
        case TexelFilter::Nearest:
            return "TexelFilter::Nearest";
        case TexelFilter::Linear:
            return "TexelFilter::Linear";
        case TexelFilter::Cubic:
            return "TexelFilter::Cubic";
        default:
            break;
    }

    return fmt::string("TexelFilter::[{}]", static_cast<std::underlying_type_t<TexelFilter>>(type));
}

template<>
inline std::string value(TexelAddressing const& type) {
    switch (type) {
        case TexelAddressing::Repeat:
            return "TexelAddressing::Repeat";
        case TexelAddressing::MirroredRepeat:
            return "TexelAddressing::MirroredRepeat";
        case TexelAddressing::ClampToEdge:
            return "TexelAddressing::ClampToEdge";
        case TexelAddressing::ClampToBorder:
            return "TexelAddressing::ClampToBorder";
        case TexelAddressing::MirrorClampToEdge:
            return "TexelAddressing::MirrorClampToEdge";
        default:
            break;
    }

    return fmt::string("TexelAddressing::[{}]", static_cast<std::underlying_type_t<TexelAddressing>>(type));
}

template<>
inline std::string value(ShaderStageFlags const& flags) {
    std::stringstream ss = {};
    ss << "ShaderStageFlags::[";

    if ((flags & ShaderStageFlags::Vertex) != ShaderStageFlags::None) {
        ss << "Vertex,";
    }

    if ((flags & ShaderStageFlags::Hull) != ShaderStageFlags::None) {
        ss << "Hull,";
    }

    if ((flags & ShaderStageFlags::Domain) != ShaderStageFlags::None) {
        ss << "Domain,";
    }

    if ((flags & ShaderStageFlags::Geometry) != ShaderStageFlags::None) {
        ss << "Geometry,";
    }

    if ((flags & ShaderStageFlags::Fragment) != ShaderStageFlags::None) {
        ss << "Fragment,";
    }

    if ((flags & ShaderStageFlags::Compute) != ShaderStageFlags::None) {
        ss << "Compute,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(DescriptorFlags const& flags) {
    std::stringstream ss = {};
    ss << "DescriptorFlags::[";

    if ((flags & DescriptorFlags::Sampler) != DescriptorFlags::None) {
        ss << "Sampler,";
    }

    if ((flags & DescriptorFlags::Uniform) != DescriptorFlags::None) {
        ss << "Uniform,";
    }

    if ((flags & DescriptorFlags::Storage) != DescriptorFlags::None) {
        ss << "Storage,";
    }

    if ((flags & DescriptorFlags::Texture) != DescriptorFlags::None) {
        ss << "Texture,";
    }

    if ((flags & DescriptorFlags::Buffer) != DescriptorFlags::None) {
        ss << "Buffer,";
    }

    if ((flags & DescriptorFlags::Texel) != DescriptorFlags::None) {
        ss << "Texel,";
    }

    if ((flags & DescriptorFlags::Input) != DescriptorFlags::None) {
        ss << "Input,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(DescriptorSetLayoutFlags const& flags) {
    std::stringstream ss = {};
    ss << "DescriptorSetLayoutFlags::[";

    if ((flags & DescriptorSetLayoutFlags::PushDescriptor) != DescriptorSetLayoutFlags::None) {
        ss << "PushDescriptor,";
    }

    if ((flags & DescriptorSetLayoutFlags::UpdateAfterBindPool) != DescriptorSetLayoutFlags::None) {
        ss << "UpdateAfterBindPool,";
    }

    if ((flags & DescriptorSetLayoutFlags::HostOnlyPool) != DescriptorSetLayoutFlags::None) {
        ss << "HostOnlyPool,";
    }

    if ((flags & DescriptorSetLayoutFlags::DescriptorBuffer) != DescriptorSetLayoutFlags::None) {
        ss << "DescriptorBuffer,";
    }

    if ((flags & DescriptorSetLayoutFlags::EmbeddedImmutableSamplers) != DescriptorSetLayoutFlags::None) {
        ss << "EmbeddedImmutableSamplers,";
    }

    if ((flags & DescriptorSetLayoutFlags::PerStageNV) != DescriptorSetLayoutFlags::None) {
        ss << "PerStageNV,";
    }

    if ((flags & DescriptorSetLayoutFlags::IndirectBindableNV) != DescriptorSetLayoutFlags::None) {
        ss << "IndirectBindableNV,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(DescriptorPoolFlags const& flags) {
    std::stringstream ss = {};
    ss << "DescriptorPoolFlags::[";

    if ((flags & DescriptorPoolFlags::FreeDescriptorSet) != DescriptorPoolFlags::None) {
        ss << "FreeDescriptorSet,";
    }

    if ((flags & DescriptorPoolFlags::UpdateAfterBind) != DescriptorPoolFlags::None) {
        ss << "UpdateAfterBind,";
    }

    if ((flags & DescriptorPoolFlags::HostOnly) != DescriptorPoolFlags::None) {
        ss << "HostOnly,";
    }

    if ((flags & DescriptorPoolFlags::AllowOverallocationSetsNV) != DescriptorPoolFlags::None) {
        ss << "AllowOverallocationSetsNV,";
    }

    if ((flags & DescriptorPoolFlags::AllowOverallocationPoolsNV) != DescriptorPoolFlags::None) {
        ss << "AllowOverallocationPoolsNV,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(PipelineShaderFlags const& flags) {
    std::stringstream ss = {};
    ss << "PipelineShaderFlags::[";

    if ((flags & PipelineShaderFlags::AllowVaryingSubgroupSize) != PipelineShaderFlags::None) {
        ss << "AllowVaryingSubgroupSize,";
    }

    if ((flags & PipelineShaderFlags::RequireFullSubgroups) != PipelineShaderFlags::None) {
        ss << "RequireFullSubgroups,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(PipelineFlags const& flags) {
    std::stringstream ss = {};
    ss << "PipelineFlags::[";

    if ((flags & PipelineFlags::DisableOptimization) != PipelineFlags::None) {
        ss << "DisableOptimization,";
    }

    if ((flags & PipelineFlags::AllowDerivatives) != PipelineFlags::None) {
        ss << "AllowDerivatives,";
    }

    if ((flags & PipelineFlags::Derivative) != PipelineFlags::None) {
        ss << "Derivative,";
    }

    if ((flags & PipelineFlags::ViewIndexFromDeviceIndex) != PipelineFlags::None) {
        ss << "ViewIndexFromDeviceIndex,";
    }

    if ((flags & PipelineFlags::DispatchBase) != PipelineFlags::None) {
        ss << "DispatchBase,";
    }

    if ((flags & PipelineFlags::FailOnPipelineCompileRequired) != PipelineFlags::None) {
        ss << "FailOnPipelineCompileRequired,";
    }

    if ((flags & PipelineFlags::EarlyReturnOnFailure) != PipelineFlags::None) {
        ss << "EarlyReturnOnFailure,";
    }

    if ((flags & PipelineFlags::NoProtectedAccess) != PipelineFlags::None) {
        ss << "NoProtectedAccess,";
    }

    if ((flags & PipelineFlags::ProtectedAccessOnly) != PipelineFlags::None) {
        ss << "ProtectedAccessOnly,";
    }

    ss << "]";
    return ss.str();
}

template<>
inline std::string value(IndexType const& type) {
    switch (type) {
        case IndexType::Uint16:
            return "IndexType::Uint16";
        case IndexType::Uint32:
            return "IndexType::Uint32";
        case IndexType::None:
            return "IndexType::None";
        case IndexType::Uint8:
            return "IndexType::Uint8";
        default:
            break;
    }

    return fmt::string("IndexType::[{}]", static_cast<std::underlying_type_t<IndexType>>(type));
}

template<>
inline std::string value(StencilFaceFlags const& flags) {
    std::stringstream ss = {};
    ss << "StencilFaceFlags::[";

    if ((flags & StencilFaceFlags::Front) != StencilFaceFlags::None) {
        ss << "Front,";
    }

    if ((flags & StencilFaceFlags::Back) != StencilFaceFlags::None) {
        ss << "Back,";
    }

    ss << "]";
    return ss.str();
}

}

}

}
