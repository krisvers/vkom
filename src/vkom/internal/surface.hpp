#pragma once

#include <vkom/surface.hpp>

#include <vkom/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

class VulkanSurface final : virtual public ISurface, virtual public CollectedByHeap {
private:
    bool _inheritedHandle = false;
    IInstance* _instance = nullptr;
    SurfaceWSIInfo _info;
    VulkanSurfaceData _surfaceData;

public:
    VulkanSurface(bool inheritedHandle, IInstance* instance, SurfaceWSIInfo const& info, VulkanSurfaceData const& surfaceData);
    ~VulkanSurface();

    /* ISurface */
    void getInfo(SurfaceWSIInfo* info) const noexcept override;

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    void const* vkData() const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

}

}
