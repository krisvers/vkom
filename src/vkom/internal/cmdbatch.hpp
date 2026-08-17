#pragma once

#include <vkom/enums.hpp>
#include <vkom/cmdbatch.hpp>

#include <vkom/cmdencoder.hpp>
#include <vkom/queue.hpp>
#include <vkom/device.hpp>
#include <vkom/adapter.hpp>
#include <vkom/instance.hpp>

#include <vkom/internal/object.hpp>
#include <vkom/internal/vulkan.hpp>
#include <vkom/internal/funcptrs.hpp>
#include <vkom/internal/vkdata.hpp>

namespace vkom {

namespace internal {

class VulkanCommandBatch final : virtual public ICommandBatch, virtual public DiscardableByHeap {
private:
    bool _inheritedHandle = false;
    IQueue* _queue = nullptr;
    IDevice* _device = nullptr;
    IAdapter* _adapter = nullptr;
    IInstance* _instance = nullptr;
    VulkanCommandBatchData _batchData;

    bool _ended = false;

public:
    VulkanCommandBatch(bool inheritedHandle, IQueue* queue, VulkanCommandBatchData const& batchData);
    ~VulkanCommandBatch();

    /* ICommandBatch */
    Result record(ICommandEncoder* encoder) noexcept override;
    Result submit(CommandBatchSubmitInfo const* submitInfo) noexcept override;

    /* IHandled */
    uint64_t handle() const noexcept override;
    ObjectType handleType() const noexcept override;

    void const* vkData() const noexcept override;

    /* IChild */
    IParent* parent() const noexcept override;

    /* IDispatchable */
    void* loadDispatchSymbol(const char* symbol) override;

    /* IInterface */
    void* queryInterface(IID const& iid) noexcept override;
};

}

}
