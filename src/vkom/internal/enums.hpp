#pragma once

#include <type_traits>

#include <vkom/enums.hpp>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

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
#else
#define VKOM_INTERNAL_ENUM_DEFINE_CAST(vkT_, vkomT_) \
inline vkT_ castEnum(vkomT_ v) { \
    return static_cast<vkT_>(static_cast<std::underlying_type_t<vkomT_>>(v)); \
} \
inline vkomT_ castEnum(vkT_ v) { \
    return static_cast<vkomT_>(static_cast<std::underlying_type_t<vkT_>>(v)); \
}
#endif

namespace vkom {

namespace internal {

VKOM_INTERNAL_ENUM_DEFINE_CAST(VkObjectType, ObjectType)
VKOM_INTERNAL_ENUM_DEFINE_CAST(VkResult, Result)

}

}
