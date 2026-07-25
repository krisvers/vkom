#pragma once

#include <type_traits>

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

}

}
