#include <vkom/enums.hpp>
#include <vkom/platform.hpp>
#include <vkom/instance.hpp>
#include <vkom/adapter.hpp>
#include <vkom/device.hpp>
#include <vkom/heap.hpp>
#include <vkom/buffer.hpp>
#include <vkom/texture.hpp>
#include <vkom/queue.hpp>
#include <vkom/cmdencoder.hpp>
#include <vkom/cmdbatch.hpp>

#include <cstdio>
#include <vector>

struct AutoReleasePool {
    std::vector<vkom::IInterface*> objects = {};

    ~AutoReleasePool() {
        for (size_t i = 0; i < objects.size(); i += 1) {
            vkom::IInterface* object = objects[objects.size() - i - 1];
            vkom::ICollected* collected = object->queryInterface<vkom::ICollected>();
            if (collected != nullptr) {
                collected->release();
                continue;
            }

            vkom::IDestructible* destructible = object->queryInterface<vkom::IDestructible>();
            if (destructible != nullptr) {
                destructible->destroy();
                continue;
            }

            vkom::IDiscardable* discardable = object->queryInterface<vkom::IDiscardable>();
            if (discardable != nullptr) {
                discardable->discard();
                continue;
            }
        }
    }

    void push(vkom::IInterface* object) {
        objects.push_back(object);
    }
};

int main(int argc, char** argv) {
    AutoReleasePool pool = {};

    vkom::InstanceLoaderInfo loaderInfo = {};

    vkom::IInstance* instance;
    if (vkom::createInstance(true, &loaderInfo, &instance) != vkom::Result::Success) {
        return 1;
    }

    pool.push(instance);

    instance->setLogCallback([](vkom::IInstance* instance, void* userData, vkom::DebugMessageSeverityFlags severity, vkom::DebugMessageTypeFlags type, const char* message) {
        std::printf("[vkom]: %s\n", message);
    }, nullptr);

    vkom::IAdapter* adapter = instance->enumerateAdapters(0);

    vkom::IDevice* device;
    if (adapter->createDevice(&device) != vkom::Result::Success) {
        return 1;
    }

    pool.push(device);

    vkom::IHeap* heap;
    if (device->createHeap(vkom::BufferUsageFlags::TransferSource | vkom::BufferUsageFlags::TransferDestination | vkom::BufferUsageFlags::StorageBuffer | vkom::BufferUsageFlags::ShaderDeviceAddress, vkom::TextureUsageFlags::None, vkom::MemoryLocationFlags::GPU, &heap) != vkom::Result::Success) {
        return 1;
    }

    pool.push(heap);

    vkom::BufferInfo bufferInfo = {};
    bufferInfo.size = 1024;
    bufferInfo.location = vkom::MemoryLocationFlags::GPU;
    bufferInfo.usage = vkom::BufferUsageFlags::TransferSource | vkom::BufferUsageFlags::TransferDestination | vkom::BufferUsageFlags::StorageBuffer | vkom::BufferUsageFlags::ShaderDeviceAddress;

    vkom::IBuffer* buffer;
    if (heap->createBuffer(&bufferInfo, &buffer) != vkom::Result::Success) {
        return 1;
    }

    pool.push(buffer);

    vkom::IQueue* queue;
    if (device->acquireQueue(vkom::QUEUE_FAMILY_ANY, vkom::QueueFlags::Graphics | vkom::QueueFlags::Present, &queue) != vkom::Result::Success) {
        return 1;
    }

    pool.push(queue);

    vkom::ICommandEncoder* encoder;
    if (queue->acquireCommandEncoder(&encoder) != vkom::Result::Success) {
        return 1;
    }

    pool.push(encoder);

    encoder->insertDebugLabel("vkom was here");

    vkom::BufferFill fill = {};
    fill.size = bufferInfo.size;
    fill.dstOffset = 0;
    fill.word = 0xb00bcafe;

    encoder->fillBuffer(buffer, &fill);

    vkom::ICommandBatch* batch;
    if (encoder->batch(&batch) != vkom::Result::Success) {
        return 1;
    }

    pool.push(batch);

    while (true) {
        if (batch->submit(nullptr) != vkom::Result::Success) {
            return 1;
        }
    }

    return 0;
}
