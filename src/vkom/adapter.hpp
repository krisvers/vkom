#pragma once

#include <vkom/enums.hpp>
#include <vkom/object.hpp>
#include <vkom/device.hpp>

namespace vkom {

struct AdapterInfo {
    VendorID vendorID;
    DriverID driverID;
    uint32_t deviceID;
    uint32_t driverVersion;
    char deviceName[256];
    char driverName[256];
};

struct AdapterFeatures {
    bool swapchain;
    bool dynamicRendering;
    bool bufferDeviceAddress;

    bool shaderInt8;
    bool shaderInt16;
    bool shaderInt64;
    bool shaderFloat8;
    bool shaderFloat16;
    bool shaderFloat64;
};

struct AdapterLimits {
    uint32_t maxTextureWidth1D;
    uint32_t maxTextureWidthHeight2D;
    uint32_t maxTextureWidthHeightDepth3D;
    uint32_t maxTextureWidthHeightCube;
    uint32_t maxTextureLayers;

    uint32_t maxUniformBufferRange;
    uint32_t maxStorageBufferRange;
    uint32_t maxPushConstantsSize;

    uint32_t maxComputeSharedMemorySize;
    uint32_t maxComputeWorkGroupCount[3];
    uint32_t maxComputeWorkGroupInvocations;
    uint32_t maxComputeWorkGroupSize[3];

    float timestampPeriod;

    uint64_t availablePrivateMemory;
    uint64_t availableSharedMemory;
    uint64_t availableHostMemory;
};

struct AdapterQueueInfo {

};

inline const IID IADAPTER_IID = IID("4a7e1c14-4f8f-4455-858b-f4f171b411f9");

class IAdapter : public IHandled, public IChild, public IParent, public IDispatchable {
public:
    virtual void queryInfo(AdapterInfo* info) const noexcept = 0;
    virtual void queryFeatures(AdapterFeatures* features) const noexcept = 0;
    virtual void queryLimits(AdapterLimits* limits) const noexcept = 0;

    virtual Result createDevice(IDevice** device) = 0;

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

}
