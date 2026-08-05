#include <vkom/enums.hpp>
#include <vkom/platform.hpp>
#include <vkom/instance.hpp>
#include <vkom/adapter.hpp>
#include <vkom/device.hpp>
#include <vkom/queue.hpp>
#include <vkom/cmdencoder.hpp>
#include <vkom/cmdbatch.hpp>

#include <cstdio>

int main(int argc, char** argv) {
    vkom::InstanceLoaderInfo loaderInfo = {};

    vkom::IInstance* instance;
    if (vkom::createInstance(true, &loaderInfo, &instance) != vkom::Result::Success) {
        return 1;
    }

    instance->setLogCallback([](vkom::IInstance* instance, void* userData, vkom::DebugMessageSeverityFlags severity, vkom::DebugMessageTypeFlags type, const char* message) {
        std::printf("[vkom]: %s\n", message);
    }, nullptr);

    vkom::IAdapter* adapter = instance->enumerateAdapters(0);

    vkom::IDevice* device;
    if (adapter->createDevice(&device) != vkom::Result::Success) {
        instance->release();
        return 1;
    }

    vkom::IQueue* queue;
    if (device->acquireQueue(vkom::QUEUE_FAMILY_ANY, vkom::QueueFlags::Graphics | vkom::QueueFlags::Present, &queue) != vkom::Result::Success) {
        device->release();
        instance->release();
        return 1;
    }

    vkom::ICommandEncoder* encoder;
    if (queue->acquireCommandEncoder(&encoder) != vkom::Result::Success) {
        queue->release();
        device->release();
        instance->release();
        return 1;
    }

    encoder->insertDebugLabel("vkom was here");

    vkom::ICommandBatch* batch;
    if (encoder->batch(&batch) != vkom::Result::Success) {
        encoder->release();
        queue->release();
        device->release();
        instance->release();
        return 1;
    }

    if (batch->submit(nullptr) != vkom::Result::Success) {

    }

    batch->discard();
    encoder->release();

    queue->release();
    device->release();
    instance->release();

    return 0;
}
