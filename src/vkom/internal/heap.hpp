#pragma once

#include <vector>

#include <vkom/enums.hpp>
#include <vkom/heap.hpp>

#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/vma.hpp>

namespace vkom {

namespace internal {

class VulkanInstance;
class VulkanAdapter;
class VulkanDevice;

class VulkanHeap final : public IHeap {
private:
    bool _debug = false;
    bool _inheritedHandle = false;
    VulkanDevice* _device = nullptr;
    VulkanAdapter* _adapter = nullptr;
    VulkanInstance* _instance = nullptr;
    VmaPool _vmaPool = nullptr;
    VkAllocationCallbacks const* _vkAllocationCallbacks = nullptr;

    /* ICollected */
    uint32_t _referenceCount = 1;

    /* IParent */
    std::vector<IChild*> _children = {};

public:
    VulkanHeap(bool debug, bool inheritedHandle, VulkanDevice* device, VmaPool vmaPool, VkAllocationCallbacks const* vkAllocationCallbacks);
    ~VulkanHeap();

    /* IHeap */
    Result createBuffer(BufferInfo const* info, IBuffer** buffer) noexcept override;
    Result createAliasedBuffer(BufferInfo const* info, ResourceAliasingInfo const* aliasingInfo, IBuffer** buffer) noexcept override;

    Result createTexture(TextureInfo const* info, ITexture** texture) noexcept override;
    Result createAliasedTexture(TextureInfo const* info, ResourceAliasingInfo const* aliasingInfo, ITexture** texture) noexcept override;

    /* INullable */
    bool isNull() const noexcept override;

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    /* ICollected */
    uint32_t release() override;
    uint32_t retain() override;

    /* IParent */
    bool hasChild(IChild const* child) const noexcept override;
    IChild* enumerateChildren(uint32_t id) const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

}

}
