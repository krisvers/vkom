#pragma once

#include <cstdint>
#include <type_traits>

#define VKOM_DEFINE_ENUM_BITFLAGS_OPERATOR_BINARY(T_, op_) \
inline T_ operator op_(T_ const& a, T_ const& b) { \
    return static_cast<T_>(static_cast<std::underlying_type_t<T_>>(a) op_ static_cast<std::underlying_type_t<T_>>(b)); \
}

#define VKOM_DEFINE_ENUM_BITFLAGS_OPERATOR_UNARY(T_, op_) \
inline T_ operator op_(T_ const& a) { \
    return static_cast<T_>(op_ static_cast<std::underlying_type_t<T_>>(a)); \
}

#define VKOM_DEFINE_ENUM_BITFLAGS_OPERATOR_BINARY_ASSIGNMENT(T_, op_, opeq_) \
inline T_& operator opeq_(T_& a, T_ const& b) { \
    return (a = static_cast<T_>(static_cast<std::underlying_type_t<T_>>(a) op_ static_cast<std::underlying_type_t<T_>>(b))); \
}

#define VKOM_DEFINE_ENUM_BITFLAGS_OPERATOR_UNARY_ASSIGNMENT(T_, op_, opeq_) \
inline T_& operator opeq_(T_& a) { \
    return (a = static_cast<T_>(op_ static_cast<std::underlying_type_t<T_>>(a))); \
}

#define VKOM_DEFINE_ENUM_BITFLAGS_OPERATORS(T_) \
    VKOM_DEFINE_ENUM_BITFLAGS_OPERATOR_UNARY(T_, ~) \
    VKOM_DEFINE_ENUM_BITFLAGS_OPERATOR_BINARY(T_, |) \
    VKOM_DEFINE_ENUM_BITFLAGS_OPERATOR_BINARY_ASSIGNMENT(T_, |, |=) \
    VKOM_DEFINE_ENUM_BITFLAGS_OPERATOR_BINARY(T_, &) \
    VKOM_DEFINE_ENUM_BITFLAGS_OPERATOR_BINARY_ASSIGNMENT(T_, &, &=) \
    VKOM_DEFINE_ENUM_BITFLAGS_OPERATOR_BINARY(T_, ^) \
    VKOM_DEFINE_ENUM_BITFLAGS_OPERATOR_BINARY_ASSIGNMENT(T_, ^, ^=)

namespace vkom {

enum class Result : int32_t {
    /* standard */
    Success = 0,
    NotReady = 1,
    Timeout = 2,
    EventSet = 3,
    EventReset = 4,
    Incomplete = 5,

    ErrorOutOfHostMemory = -1,
    ErrorOutOfDeviceMemory = -2,
    ErrorInitializationFailed = -3,
    ErrorDeviceLost = -4,
    ErrorMemoryMapFailed = -5,
    ErrorLayerNotPresent = -6,
    ErrorExtensionNotPresent = -7,
    ErrorFeatureNotPresent = -8,
    ErrorIncompatibleDriver = -9,
    ErrorTooManyObjects = -10,
    ErrorFormatNotSupported = -11,
    ErrorFragmentedPool = -12,
    ErrorUnknown = -13,

    /* vkom-specific */
    ErrorUnsupportedFeature = -256,

    /* extensions */
    SuboptimalSwapchain = 1000001003,
    ErrorSurfaceLost = -1000000000,
    ErrorOutOfDateSwapchain = -1000001004,
    ErrorValidationFailed = -1000011001,
};

enum class ObjectType : int32_t {
    /* standard */
    Unknown = 0,
    Instance = 1,
    PhysicalDevice = 2,
    Device = 3,
    Queue = 4,
    Semaphore = 5,
    CommandBuffer = 6,
    Fence = 7,
    DeviceMemory = 8,
    Buffer = 9,
    Image = 10,
    Event = 11,
    QueryPool = 12,
    BufferView = 13,
    ImageView = 14,
    ShaderModule = 15,
    PipelineCache = 16,
    PipelineLayout = 17,
    RenderPass = 18,
    Pipeline = 19,
    DescriptorSetLayout = 20,
    Sampler = 21,
    DescriptorPool = 22,
    DescriptorSet = 23,
    Framebuffer = 24,
    CommandPool = 25,

    /* extensions */
    SurfaceKHR = 1000000000,
    SwapchainKHR = 1000001000,
    DisplayKHR = 1000002000,
    DisplayModeKHR = 1000002001,
    DebugUtilsMessengerEXT = 1000128000,
};

enum class DebugMessageSeverityFlags : int32_t {
    Verbose = 0x0001,
    Info = 0x0010,
    Warning = 0x0100,
    Error = 0x1000,
};

VKOM_DEFINE_ENUM_BITFLAGS_OPERATORS(DebugMessageSeverityFlags)

enum class DebugMessageTypeFlags : int32_t {
    General = 0x0001,
    Validation = 0x0002,
    Performance = 0x0004,
    DeviceAddressBinding = 0x0008,
};

VKOM_DEFINE_ENUM_BITFLAGS_OPERATORS(DebugMessageTypeFlags)

enum class VendorID : uint32_t {
    Unknown = 0x0000,
    AMD = 0x1002,
    ImgTec = 0x1010,
    Apple = 0x106B,
    Nvidia = 0x10DE,
    ARM = 0x13B5,
    Broadcom = 0x14E4,
    Vivante = 0x1D4E,
    Qualcomm = 0x5143,
    Intel = 0x8086,
    Mesa = 0x10005,
};

enum class DriverID : uint32_t {
    Unknown = 0,
    AMDProprietary = 1,
    AMDOpenSource = 2,
    MesaRADV = 3,
    NvidiaProprietary = 4,
    IntelProprietaryWindows = 5,
    IntelOpenSourceMesa = 6,
    ImaginationProprietary = 7,
    QualcommProprietary = 8,
    ARMProprietary = 9,
    GoogleSwiftShader = 10,
    GGPProprietary = 11,
    BroadcomProprietary = 12,
    MesaLLVMPipe = 13,
    MoltenVK = 14,
    CoreAVIProprietary = 15,
    JuiceProprietary = 16,
    VerisiliconProprietary = 17,
    MesaTurnip = 18,
    MesaV3DV = 19,
    MesaPanVK = 20,
    SamsungProprietary = 21,
    MesaVenus = 22,
    MesaDozen = 23,
    MesaNVK = 24,
    ImaginationOpenSourceMesa = 25,
    MesaHoneyKrisp = 26,
    VulkanSCEmulationOnVulkan = 27,
    MesaKosmicKrisp = 28,
};

enum class AdapterType : uint32_t {
    Other = 0,
    Integrated = 1,
    Dedicated = 2,
    Virtual = 3,
    CPU = 4,
};

enum class QueueFlags : int32_t {
    Graphics = 0x00000001,
    Transfer = 0x00000002,
    Compute = 0x00000004,
    SparseBinding = 0x00000008,
    Protected = 0x00000010,
    VideoDecode = 0x00000020,
    VideoEncode = 0x00000040,
    Present = 0x40000000,
};

VKOM_DEFINE_ENUM_BITFLAGS_OPERATORS(QueueFlags)

enum class MemoryLocationFlags : uint32_t {
    None = 0x00,
    GPU = 0x01,
    CPU = 0x02,
};

VKOM_DEFINE_ENUM_BITFLAGS_OPERATORS(MemoryLocationFlags)

}
