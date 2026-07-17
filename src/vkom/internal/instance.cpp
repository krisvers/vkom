#include <vkom/instance.hpp>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

namespace vkom {

namespace internal {

class VulkanInstance : public IInstance {
private:
    VkInstance _vkInstance = nullptr;

public:

};

}

Result createInstance(bool debug, const InstanceLoaderInfo *loaderInfo, IInstance **instance) {

}

}
