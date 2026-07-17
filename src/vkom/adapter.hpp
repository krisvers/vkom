#pragma once

#include <vkom/object.hpp>

namespace vkom {

struct AdapterFeatures {
    bool swapchain;
    bool dynamicRendering;
    bool bufferDeviceAddress;
};

struct AdapterLimits {
    uint32_t maxTextureWidth1D;
    uint32_t maxTextureWidthHeight2D;
    uint32_t maxTextureWidthHeightDepth3D;
    uint32_t maxTextureWidthHeightCube;
    uint32_t maxTextureArrayLayers;

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
    uint64_t availableSystemMemory;
};

inline const IID IADAPTER_IID = IID("4a7e1c14-4f8f-4455-858b-f4f171b411f9");

class IAdapter : virtual public IHandled, IChild, IParent {
public:
    virtual void queryFeatures(AdapterFeatures* features) = 0;

    /* IInterface */
    bool supportsInterface(IID const& iid) const noexcept override;
};

}
