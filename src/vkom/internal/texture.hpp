#pragma once

#include <vkom/enums.hpp>
#include <vkom/texture.hpp>

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

class VulkanTexture final : virtual public ITexture, virtual public ParentByVector, virtual public CollectedByHeap {
private:
    bool _inheritedHandle = false;
    bool _alias = false;
    IHeap* _heap = nullptr;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    TextureInfo _info = {};
    VulkanTextureData _textureData;

public:
    VulkanTexture(bool inheritedHandle, bool alias, IHeap* heap, TextureInfo const& info, VulkanTextureData const& textureData);
    ~VulkanTexture();

    /* ITexture */
    void getInfo(TextureInfo* info) const noexcept override;

    Result createView(TextureViewInfo const* info, ITextureView** view) noexcept override;

    /* IResource */
    bool isAlias() const noexcept override;
    void getAllocationInfo(ResourceAllocationInfo* info) const noexcept override;

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
