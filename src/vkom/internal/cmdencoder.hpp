#pragma once

#include <vector>

#include <vkom/enums.hpp>
#include <vkom/cmdencoder.hpp>

#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vulkan.hpp>

namespace vkom {

namespace internal {

struct VulkanCommandEncoderFunctionPointers {
    CommandBufferFunctionPointers10 commandBuffer10;
};

class VulkanInstance;
class VulkanAdapter;
class VulkanDevice;
class VulkanQueue;

class VulkanCommandEncoder final : public ICommandEncoder {
private:
    bool _debug = false;
    bool _inheritedHandle = false;
    VulkanQueue* _queue = nullptr;
    VulkanDevice* _device = nullptr;
    VulkanAdapter* _adapter = nullptr;
    VulkanInstance* _instance = nullptr;
};

}

}