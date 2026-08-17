#pragma once

#include <vkom/enums.hpp>
#include <vkom/heap.hpp>

#include <vkom/device.hpp>
#include <vkom/adapter.hpp>
#include <vkom/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vma.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

class VulkanHeap final : virtual public IHeap, virtual public ParentByVector, virtual public CollectedByHeap {
private:
    bool _inheritedHandle = false;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanHeapData _heapData;

public:
    VulkanHeap(bool inheritedHandle, IDevice* device, VulkanHeapData const& heapData);
    ~VulkanHeap();

    /* IHeap */
    Result createBuffer(BufferInfo const* info, IBuffer** buffer) noexcept override;
    Result createAliasedBuffer(BufferInfo const* info, ResourceAliasingInfo const* aliasingInfo, IBuffer** buffer) noexcept override;

    Result createTexture(TextureInfo const* info, ITexture** texture) noexcept override;
    Result createAliasedTexture(TextureInfo const* info, ResourceAliasingInfo const* aliasingInfo, ITexture** texture) noexcept override;

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
