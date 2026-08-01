#include <vkom/internal/cmdencoder.hpp>

#include <vkom/internal/enums.hpp>
#include <vkom/internal/cmdpasses.hpp>
#include <vkom/internal/queue.hpp>
#include <vkom/internal/device.hpp>
#include <vkom/internal/adapter.hpp>
#include <vkom/internal/instance.hpp>

namespace vkom {

namespace internal {

VulkanCommandEncoder::VulkanCommandEncoder(bool debug, bool inheritedHandle, VulkanQueue* queue, VkCommandBuffer vkCommandBuffer, VkAllocationCallbacks const* vkAllocationCallbacks, VulkanCommandEncoderFunctionPointers const& functionPointers) : _debug(debug), _inheritedHandle(inheritedHandle), _queue(queue), _device(static_cast<VulkanDevice*>(_queue->parent())), _adapter(static_cast<VulkanAdapter*>(_device->parent())), _instance(static_cast<VulkanInstance*>(_adapter->parent())), _vkCommandBuffer(vkCommandBuffer), _vkAllocationCallbacks(vkAllocationCallbacks), _functionPointers(functionPointers) {
    if (functionPointers.commandBuffer10.vkResetCommandBuffer(_vkCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset command buffer");
    }

    /* TODO: some form of render pass/sub pass inheritance */
    VkCommandBufferInheritanceInfo inheritanceInfo = {};
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pInheritanceInfo = &inheritanceInfo;

    if (functionPointers.commandBuffer10.vkBeginCommandBuffer(_vkCommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer");
    }
}

VulkanCommandEncoder::~VulkanCommandEncoder() {
    if (_renderPass != nullptr) {
        _renderPass->end();
    }

    for (IChild* child : _children) {
        if (child->supportsInterface(ICOLLECTED_IID)) {
            ICollected* collected = reinterpret_cast<ICollected*>(child);
            if (collected->release() != 0) {
                /* TODO: report mismanaged references */
            }
        }
    }

    if (!_ended && _functionPointers.commandBuffer10.vkEndCommandBuffer(_vkCommandBuffer) != VK_SUCCESS) {

    }

    _queue->releaseCommandBuffer(_vkCommandBuffer);
}

/* ICommandEncoder */
Result VulkanCommandEncoder::beginComputePass(ComputePassDescriptor const* descriptor, IComputePass** pass) noexcept {
    return Result::ErrorInitializationFailed;
}

Result VulkanCommandEncoder::beginRenderPass(RenderPassDescriptor const* descriptor, IRenderPass** pass) noexcept {
    return Result::ErrorInitializationFailed;
}

void VulkanCommandEncoder::insertDebugLabel(const char* label) noexcept {
    if (_functionPointers.debugUtilsEXT.vkCmdInsertDebugUtilsLabelEXT == nullptr) {
        return;
    }

    VkDebugUtilsLabelEXT labelInfo = {};
    labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    labelInfo.pLabelName = label;
    labelInfo.color[3] = 1.0f;

    hashLabelToColor(label, labelInfo.color[0], labelInfo.color[1], labelInfo.color[2]);
    _functionPointers.debugUtilsEXT.vkCmdInsertDebugUtilsLabelEXT(_vkCommandBuffer, &labelInfo);
}

void VulkanCommandEncoder::pushDebugGroup(const char* label) noexcept {
    if (_functionPointers.debugUtilsEXT.vkCmdBeginDebugUtilsLabelEXT == nullptr) {
        return;
    }

    VkDebugUtilsLabelEXT labelInfo = {};
    labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    labelInfo.pLabelName = label;
    labelInfo.color[3] = 1.0f;

    hashLabelToColor(label, labelInfo.color[0], labelInfo.color[1], labelInfo.color[2]);
    _functionPointers.debugUtilsEXT.vkCmdBeginDebugUtilsLabelEXT(_vkCommandBuffer, &labelInfo);
}

void VulkanCommandEncoder::popDebugGroup() noexcept {
    if (_functionPointers.debugUtilsEXT.vkCmdEndDebugUtilsLabelEXT == nullptr) {
        return;
    }

    _functionPointers.debugUtilsEXT.vkCmdEndDebugUtilsLabelEXT(_vkCommandBuffer);
}

void VulkanCommandEncoder::copyBufferToBuffer() noexcept {

}

void VulkanCommandEncoder::copyBufferToImage() noexcept {

}

void VulkanCommandEncoder::copyImageToImage() noexcept {

}

void VulkanCommandEncoder::copyImageToBuffer() noexcept {

}

void VulkanCommandEncoder::smallBufferUpload() noexcept {

}

void VulkanCommandEncoder::fillBuffer() noexcept {

}

void VulkanCommandEncoder::globalMemoryBarrier(GeneralBarrier const* barrier) noexcept {

}

void VulkanCommandEncoder::transitionTexture(TextureTransition const* transition) noexcept {

}

void VulkanCommandEncoder::transitionBuffer(BufferTransition const* transition) noexcept {

}

void VulkanCommandEncoder::transferResourceQueueFamily(ResourceQueueFamilyTransfer const* transfer) noexcept {

}

Result VulkanCommandEncoder::finish() noexcept {
    if (_ended) {
        return Result::Success;
    }

    _ended = true;
    return castEnum<Result>(_functionPointers.commandBuffer10.vkEndCommandBuffer(_vkCommandBuffer));
}

Result VulkanCommandEncoder::batch(ICommandBatch** batch) noexcept {
    Result result = finish();
    if (result != Result::Success) {
        return result;
    }

    ICommandBatch* internalBatch;
    result = _queue->acquireCommandBatch(&internalBatch);
    if (result != Result::Success) {
        return result;
    }

    result = internalBatch->record(this);
    if (result != Result::Success) {
        internalBatch->discard();
    }

    *batch = internalBatch;
    return Result::Success;
}

/* INullable */
bool VulkanCommandEncoder::isNull() const noexcept {
    return (_vkCommandBuffer != nullptr);
}

/* IHandled */
uint64_t VulkanCommandEncoder::handle() const noexcept {
    return reinterpret_cast<uint64_t>(_vkCommandBuffer);
}

ObjectType VulkanCommandEncoder::handleType() const noexcept {
    return ObjectType::CommandBuffer;
}

/* ICollected */
uint32_t VulkanCommandEncoder::release() {
    if (_referenceCount == 0) {
        return 0;
    }

    _referenceCount -= 1;
    if (_referenceCount == 0) {
        delete this;
        return 0;
    }

    return _referenceCount;
}

uint32_t VulkanCommandEncoder::retain() {
    _referenceCount += 1;
    return _referenceCount;
}

/* IParent */
bool VulkanCommandEncoder::hasChild(IChild const* child) const noexcept {
    for (IChild const* c : _children) {
        if (c == child) {
            return true;
        }
    }

    return false;
}

IChild* VulkanCommandEncoder::enumerateChildren(uint32_t id) const noexcept {
    if (id >= _children.size()) {
        return nullptr;
    }

    return _children[id];
}

/* IChild */
IParent* VulkanCommandEncoder::parent() const noexcept {
    return _queue;
}

/* IDispatchable */
void* VulkanCommandEncoder::loadDispatchSymbol(const char* symbol) {
    return _queue->loadDispatchSymbol(symbol);
}

/* IInterface */
bool VulkanCommandEncoder::supportsInterface(IID const& iid) const noexcept {
    return ICommandEncoder::supportsInterface(iid);
}

/* internal */
void VulkanCommandEncoder::hashLabelToColor(const char* label, float& r, float& g, float& b) {
    std::hash<std::string> hasher = {};
    size_t hashed = hasher(label);

    r = ((hashed & 0xffe00000) >> 21) / 2048.0f;
    g = ((hashed & 0x001ffc00) >> 10) / 2048.0f;
    b = ((hashed & 0x000003ff) >> 0) / 1024.0f;
}

}

}