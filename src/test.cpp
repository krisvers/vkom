#include <SDL3/SDL.h>

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
#include <stdexcept>
#include <vector>

#include <vkom/internal/object.hpp>

#ifdef VKOM_PLATFORM_FAMILY_APPLE
#include <SDL3/SDL_metal.h>
#endif

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

class CollectedWrapper : virtual public vkom::internal::CollectedByHeap {
public:
    void* queryInterface(vkom::IID const& iid) noexcept override {
        if (iid == IBase::iid()) {
            return static_cast<IBase*>(this);
        } else if (iid == ICollected::iid()) {
            return static_cast<ICollected*>(this);
        }

        return nullptr;
    }
};

class SDLCollected final : virtual public CollectedWrapper {
public:
    SDLCollected(SDL_InitFlags flags) {
        if (!SDL_Init(flags)) {
            throw std::runtime_error("SDL_Init failed");
        }
    }

    ~SDLCollected() {
        SDL_Quit();
    }
};

class SDLCollectedWindow final : virtual public CollectedWrapper {
private:
    SDL_Window* _window = nullptr;

    #ifdef VKOM_PLATFORM_FAMILY_APPLE
    SDL_MetalView _view = nullptr;
    #endif

public:
    SDLCollectedWindow(const char* title, uint32_t width, uint32_t height, SDL_WindowFlags flags) {
        _window = SDL_CreateWindow(title, static_cast<int>(width), static_cast<int>(height), flags);
        if (_window == nullptr) {
            throw std::runtime_error("SDL_CreateWindow failed");
        }

        #ifdef VKOM_PLATFORM_FAMILY_APPLE
        _view = SDL_Metal_CreateView(_window);
        if (_view == nullptr) {
            throw std::runtime_error("SDL_Metal_CreateView failed");
        }
        #endif
    }

    ~SDLCollectedWindow() {
        #ifdef VKOM_PLATFORM_FAMILY_APPLE
        SDL_Metal_DestroyView(_view);
        #endif

        SDL_DestroyWindow(_window);
    }

    SDL_Window* window() const noexcept {
        return _window;
    }

    void getSurfaceWSIInfo(vkom::SurfaceWSIInfo* info) {
        *info = {};
        const char* videoDriver = SDL_GetCurrentVideoDriver();

        #ifdef VKOM_PLATFORM_FAMILY_NT
        info->type = vkom::SurfaceSurfaceWSIType::Win32;
        info->windowHandle = reinterpret_cast<uint64_t>(SDL_GetPointerProperty(SDL_GetWindowProperties(_window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
        #elif defined(VKOM_PLATFORM_FAMILY_APPLE)
        info->type = vkom::SurfaceWSIType::Metal;
        info->windowHandle = reinterpret_cast<uint64_t>(SDL_Metal_GetLayer(_view));
        #elif defined(VKOM_PLATFORM_FAMILY_UNIX)
        if (std::strcmp(videoDriver, "x11") == 0) {
            info->type = vkom::SurfaceSurfaceWSIType::Xlib;
            info->windowHandle = reinterpret_cast<uint64_t>(SDL_GetPointerProperty(SDL_GetWindowProperties(_window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, nullptr));
            info->displayHandle = reinterpret_cast<uint64_t>(SDL_GetPointerProperty(SDL_GetWindowProperties(_window), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr));
        } else if (std::strcmp(videoDriver, "wayland") == 0) {
            info->type = vkom::SurfaceSurfaceWSIType::Wayland;
            info->windowHandle = reinterpret_cast<uint64_t>(SDL_GetPointerProperty(SDL_GetWindowProperties(_window), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr));
            info->displayHandle = reinterpret_cast<uint64_t>(SDL_GetPointerProperty(SDL_GetWindowProperties(_window), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr));
        }
        #endif
    }
};

int main(int argc, char** argv) {
    AutoReleasePool pool = {};

    SDLCollected* sdl = new SDLCollected(SDL_INIT_VIDEO);
    pool.push(sdl);

    SDLCollectedWindow* window = new SDLCollectedWindow("vkom test", 1200, 900, SDL_WINDOW_RESIZABLE);
    pool.push(window);

    vkom::InstanceLoaderInfo loaderInfo = {};

    vkom::IInstance* instance;
    if (vkom::createInstance(true, &loaderInfo, &instance) != vkom::Result::Success) {
        return 1;
    }

    pool.push(instance);

    instance->setLogCallback([](vkom::IInstance* instance, void* userData, vkom::DebugMessageSeverityFlags severity, vkom::DebugMessageTypeFlags type, const char* message) {
        std::printf("[vkom]: %s\n", message);
    }, nullptr);

    vkom::SurfaceWSIInfo surfaceWSIInfo;
    window->getSurfaceWSIInfo(&surfaceWSIInfo);

    vkom::ISurface* surface;
    if (instance->createSurface(&surfaceWSIInfo, &surface) != vkom::Result::Success) {
        return 1;
    }

    pool.push(surface);

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

    bool quit = false;
    while (!quit) {
        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent)) {
            switch (sdlEvent.type) {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
                default:
                    break;
            }
        }

        if (encoder->batch(&batch) != vkom::Result::Success) {
            return 2;
        }

        if (batch->submit(nullptr) != vkom::Result::Success) {
            return 3;
        }

        batch->discard();
    }

    return 0;
}
