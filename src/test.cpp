#include <vkom/instance.hpp>

int main(int argc, char** argv) {
    vkom::InstanceLoaderInfo loaderInfo = {};
    loaderInfo.loaderPath = "vulkan-1.dll";

    vkom::IInstance* instance;
    if (vkom::createInstance(true, &loaderInfo, &instance) != vkom::Result::Success) {

    }

    vkom::IAdapter* adapter = instance->enumerateAdapters(0);

    vkom::IDevice* device;
    if (adapter->createDevice(&device) != vkom::Result::Success) {

    }

    device->release();
    instance->release();
    return 0;
}
