#include <vkom/instance.hpp>

#include <cstdio>

int main(int argc, char** argv) {
    vkom::InstanceLoaderInfo loaderInfo = {};
    loaderInfo.loaderPath = "vulkan-1.dll";
    

    vkom::IInstance* instance;
    if (vkom::createInstance(true, &loaderInfo, &instance) != vkom::Result::Success) {

    }

    instance->setLogCallback([](vkom::IInstance* instance, void* userData, vkom::DebugMessageSeverityFlags severity, vkom::DebugMessageTypeFlags type, const char* message) {
        std::printf("[vkom]: %s\n", message);
    }, nullptr);

    vkom::IAdapter* adapter = instance->enumerateAdapters(0);

    vkom::IDevice* device;
    if (adapter->createDevice(&device) != vkom::Result::Success) {

    }

    device->release();
    instance->release();
    return 0;
}
