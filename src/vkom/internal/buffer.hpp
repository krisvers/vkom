#pragma once

#include <vkom/enums.hpp>
#include <vkom/buffer.hpp>

#include <vkom/heap.hpp>
#include <vkom/device.hpp>
#include <vkom/adapter.hpp>
#include <vkom/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/vma.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

class VulkanBuffer final : virtual public IBuffer, virtual public ParentByVector, virtual public CollectedByHeap {
private:
    bool _inheritedHandle = false;
    bool _alias = false;
    IHeap* _heap = nullptr;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    BufferInfo _info = {};
    VulkanBufferData _bufferData;

public:
    VulkanBuffer(bool inheritedHandle, bool alias, IHeap* heap, BufferInfo const& info, VulkanBufferData const& bufferData);
    ~VulkanBuffer();

    /* IBuffer */
    void getInfo(BufferInfo* info) const noexcept override;
    uint64_t deviceAddress() const noexcept override;

    Result createView(BufferViewInfo const* info, IBufferView** view) noexcept override;

    /* IResource */
    bool isAlias() const noexcept override;
    void getAllocationInfo(ResourceAllocationInfo* info) const noexcept override;

    void* map() noexcept override;
    void unmap() noexcept override;

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
