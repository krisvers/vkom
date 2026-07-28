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
    None = 0x0000,
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
    None = 0x00000000,
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

enum class PipelineStageFlags : uint64_t {
    None = 0x0000000000000000,
    TopOfPipe = 0x0000000000000001,
    DrawIndirect = 0x0000000000000002,
    VertexInput = 0x0000000000000004,
    VertexShader = 0x0000000000000008,
    HullShader = 0x0000000000000010,
    DomainShader = 0x0000000000000020,
    GeometryShader = 0x0000000000000040,
    FragmentShader = 0x0000000000000080,
    EarlyFragmentTests = 0x0000000000000100,
    LateFragmentTests = 0x0000000000000200,
    RenderTarget = 0x0000000000000400,
    ComputeShader = 0x0000000000000800,
    Transfer = 0x0000000000001000,
    BottomOfPipe = 0x0000000000002000,
    Host = 0x0000000000004000,
    AllGraphics = 0x0000000000008000,
    AllCommands = 0x0000000000010000,
};

VKOM_DEFINE_ENUM_BITFLAGS_OPERATORS(PipelineStageFlags)

enum class ResourceAccessFlags : uint64_t {
    None = 0x0000000000000000,
    IndirectCommandRead = 0x0000000000000001,
    IndexRead = 0x0000000000000002,
    VertexAttributeRead = 0x0000000000000004,
    UniformRead = 0x0000000000000008,
    InputAttachmentRead = 0x0000000000000010,
    ShaderRead = 0x0000000000000020,
    ShaderWrite= 0x0000000000000040,
    RenderTargetRead = 0x0000000000000080,
    RenderTargetWrite = 0x0000000000000100,
    DepthStencilTargetRead = 0x0000000000000200,
    DepthStencilTargetWrite = 0x0000000000000400,
    TransferRead = 0x0000000000000800,
    TransferWrite = 0x0000000000001000,
    HostRead = 0x0000000000002000,
    HostWrite = 0x0000000000004000,
    MemoryRead = 0x0000000000008000,
    MemoryWrite = 0x0000000000010000,
};

VKOM_DEFINE_ENUM_BITFLAGS_OPERATORS(ResourceAccessFlags)

enum class BufferUsageFlags : int32_t {
    None = 0x00000000,
    TransferSource = 0x00000001,
    TransferDestination = 0x00000002,
    UniformTexelBuffer = 0x00000004,
    StorageTexelBuffer = 0x00000008,
    UniformBuffer = 0x00000010,
    StorageBuffer = 0x00000020,
    IndexBuffer = 0x00000040,
    VertexBuffer = 0x00000080,
    IndirectBuffer = 0x00000100,

    VideoDecodeSource = 0x00002000,
    VideoDecodeDestination = 0x00004000,
    VideoEncodeSource = 0x00008000,
    VideoEncodeDestination = 0x00010000,

    ShaderDeviceAddress = 0x00020000,
};

VKOM_DEFINE_ENUM_BITFLAGS_OPERATORS(BufferUsageFlags)

enum class TextureLayout : int32_t {
    Undefined = 0,
    General = 1,
    RenderTarget = 2,
    DepthStencilTarget = 3,
    DepthStencilReadOnly = 4,
    ShaderReadOnly = 5,
    TransferSource = 6,
    TransferDestination = 7,
    Preinitialized = 8,

    PresentSource = 1000001002,
};

enum class TextureUsageFlags : int32_t {
    None = 0x00000000,
    TransferSource = 0x00000001,
    TransferDestination = 0x00000002,
    Sampled = 0x00000004,
    Storage = 0x00000008,
    RenderTarget = 0x00000010,
    DepthStencilTarget = 0x00000020,
    TransientTarget = 0x00000040,
    InputTarget = 0x00000080,

    HostTransfer = 0x00400000,

    VideoDecodeDestination = 0x00000400,
    VideoDecodeSource = 0x00000800,
    VideoDecodeDPB = 0x00001000,
    VideoEncodeDestination = 0x00002000,
    VideoEncodeSource = 0x00004000,
    VideoEncodeDPB = 0x00008000,
    FragmentShadingRateTarget = 0x00000100,
    FragmentDensityMap = 0x00000200,
};

VKOM_DEFINE_ENUM_BITFLAGS_OPERATORS(TextureUsageFlags)

enum class Format : int32_t {
    Undefined = 0,
    R8UnsignedNorm = 9,
    R8SignedNorm = 10,
    R8UnsignedInt = 13,
    R8SignedInt = 14,
    R8UnsignedNormSRGB = 15,
    RG8UnsignedNorm = 16,
    RG8SignedNorm = 17,
    RG8UnsignedInt = 20,
    RG8SignedInt = 21,
    RG8UnsignedNormSRGB = 22,
    RGB8UnsignedNorm = 23,
    RGB8SignedNorm = 24,
    RGB8UnsignedInt = 27,
    RGB8SignedInt = 28,
    RGB8UnsignedNormSRGB = 29,
    RGBA8UnsignedNorm = 37,
    RGBA8SignedNorm = 38,
    RGBA8UnsignedInt = 41,
    RGBA8SignedInt = 42,
    RGBA8UnsignedNormSRGB = 43,
    BGR8UnsignedNorm = 30,
    BGR8SignedNorm = 31,
    BGR8UnsignedInt = 34,
    BGR8SignedInt = 35,
    BGR8UnsignedNormSRGB = 36,
    BGRA8UnsignedNorm = 44,
    BGRA8SignedNorm = 45,
    BGRA8UnsignedInt = 48,
    BGRA8SignedInt = 49,
    BGRA8UnsignedNormSRGB = 50,
    R16UnsignedNorm = 70,
    R16SignedNorm = 71,
    R16UnsignedInt = 74,
    R16SignedInt = 75,
    R16SignedFloat = 76,
    RG16UnsignedNorm = 77,
    RG16SignedNorm = 78,
    RG16UnsignedInt = 81,
    RG16SignedInt = 82,
    RG16SignedFloat = 83,
    RGB16UnsignedNorm = 84,
    RGB16SignedNorm = 85,
    RGB16UnsignedInt = 88,
    RGB16SignedInt = 89,
    RGB16SignedFloat = 90,
    RGBA16UnsignedNorm = 91,
    RGBA16SignedNorm = 92,
    RGBA16UnsignedInt = 95,
    RGBA16SignedInt = 96,
    RGBA16SignedFloat = 97,
    R32UnsignedInt = 98,
    R32SignedInt = 99,
    R32SignedFloat = 100,
    RG32UnsignedInt = 101,
    RG32SignedInt = 102,
    RG32SignedFloat = 103,
    RGB32UnsignedInt = 104,
    RGB32SignedInt = 105,
    RGB32SignedFloat = 106,
    RGBA32UnsignedInt = 107,
    RGBA32SignedInt = 108,
    RGBA32SignedFloat = 109,
    R64UnsignedInt = 110,
    R64SignedInt = 111,
    R64SignedFloat = 112,
    RG64UnsignedInt = 113,
    RG64SignedInt = 114,
    RG64SignedFloat = 115,
    RGB64UnsignedInt = 116,
    RGB64SignedInt = 117,
    RGB64SignedFloat = 118,
    RGBA64UnsignedInt = 119,
    RGBA64SignedInt = 120,
    RGBA64SignedFloat = 121,
    B10G11R11UnsignedFloat = 122,
    E5B9G9R9UnsignedFloat = 123,
    Depth16UnsignedNorm = 124,
    Depth32SignedFloat = 125,
    Stencil8UnsignedInt = 126,
    Depth16UnsignedNormStencil8UnsignedInt = 128,
    Depth24UnsignedNormStencil8UnsignedInt = 129,
    Depth32SignedFloatStencil8UnsignedInt = 130,
};

}
