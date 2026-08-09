project('vkom', 'cpp', default_options: ['cpp_std=c++17'])

vma_subproject = subproject('vulkan-memory-allocator')

vk = dependency('vulkan', version: '>=1.2.0', required: true)
vma = dependency('VulkanMemoryAllocator', version: '>=3.0.0', required: true)

add_project_arguments('-DVKOM_COMPILING_DYNAMIC_LIBRARY', language: 'cpp')

vkom = shared_library('vkom',
    sources: [
        'src/vkom/platform.hpp',
        'src/vkom/interface.hpp',
        'src/vkom/uuid.hpp',
        'src/vkom/enums.hpp',
        'src/vkom/dynlib.hpp', 'src/vkom/dynlib.cpp',
        'src/vkom/object.hpp', 'src/vkom/object.cpp',
        'src/vkom/instance.hpp', 'src/vkom/instance.cpp',
        'src/vkom/adapter.hpp', 'src/vkom/adapter.cpp',
        'src/vkom/device.hpp', 'src/vkom/device.cpp',
        'src/vkom/fence.hpp', 'src/vkom/fence.cpp',
        'src/vkom/semaphore.hpp', 'src/vkom/semaphore.cpp',
        'src/vkom/surface.hpp', 'src/vkom/surface.cpp',
        'src/vkom/swapchain.hpp', 'src/vkom/swapchain.cpp',
        'src/vkom/pipeline.hpp', 'src/vkom/pipeline.cpp',
        'src/vkom/heap.hpp', 'src/vkom/heap.cpp',
        'src/vkom/resource.hpp', 'src/vkom/resource.cpp',
        'src/vkom/texture.hpp', 'src/vkom/texture.cpp',
        'src/vkom/buffer.hpp', 'src/vkom/buffer.cpp',
        'src/vkom/queue.hpp', 'src/vkom/queue.cpp',
        'src/vkom/cmdencoder.hpp', 'src/vkom/cmdencoder.cpp',
        'src/vkom/cmdpasses.hpp', 'src/vkom/cmdpasses.cpp',
        'src/vkom/cmdbatch.hpp', 'src/vkom/cmdbatch.cpp',
        'src/vkom/internal/vulkan.hpp',
        'src/vkom/internal/enums.hpp',
        'src/vkom/internal/funcptrs.hpp',
        'src/vkom/internal/vksurface.hpp',
        'src/vkom/internal/vkdata.hpp',
        'src/vkom/internal/vma.hpp', 'src/vkom/internal/vma.cpp',
        'src/vkom/internal/object.hpp', 'src/vkom/internal/object.cpp',
        'src/vkom/internal/instance.hpp', 'src/vkom/internal/instance.cpp',
        'src/vkom/internal/adapter.hpp', 'src/vkom/internal/adapter.cpp',
        'src/vkom/internal/device.hpp', 'src/vkom/internal/device.cpp',
        'src/vkom/internal/fence.hpp', 'src/vkom/internal/fence.cpp',
        'src/vkom/internal/semaphore.hpp', 'src/vkom/internal/semaphore.cpp',
        'src/vkom/internal/surface.hpp', 'src/vkom/internal/surface.cpp',
        'src/vkom/internal/swapchain.hpp', 'src/vkom/internal/swapchain.cpp',
        'src/vkom/internal/pipeline.hpp', 'src/vkom/internal/pipeline.cpp',
        'src/vkom/internal/heap.hpp', 'src/vkom/internal/heap.cpp',
        'src/vkom/internal/resource.hpp', 'src/vkom/internal/resource.cpp',
        'src/vkom/internal/texture.hpp', 'src/vkom/internal/texture.cpp',
        'src/vkom/internal/buffer.hpp', 'src/vkom/internal/buffer.cpp',
        'src/vkom/internal/queue.hpp', 'src/vkom/internal/queue.cpp',
        'src/vkom/internal/cmdencoder.hpp', 'src/vkom/internal/cmdencoder.cpp',
        'src/vkom/internal/cmdpasses.hpp', 'src/vkom/internal/cmdpasses.cpp',
        'src/vkom/internal/cmdbatch.hpp', 'src/vkom/internal/cmdbatch.cpp',
        'src/vkom/internal/nt/vksurface.cpp',
        'src/vkom/internal/nt/dynlib.hpp', 'src/vkom/internal/nt/dynlib.cpp',
        'src/vkom/internal/unix/vksurface.cpp',
        'src/vkom/internal/unix/dynlib.hpp', 'src/vkom/internal/unix/dynlib.cpp',
        'src/vkom/internal/apple/vksurface.cpp',
    ],
    include_directories: include_directories('src'),
    dependencies: [ vk, vma ],
    override_options: ['cpp_std=c++17'],
    install: true,
)

sdl3 = dependency('sdl3', version: '>=3.0.0', required: true)

executable('vkom_test',
    sources: [
        'src/test.cpp',
    ],
    include_directories: include_directories('src'),
    dependencies: [ sdl3, vma ],
    override_options: ['cpp_std=c++17'],
    link_with: [ vkom ],
)
