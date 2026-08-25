project('vkom',
    'cpp',
    license: 'MIT',
    default_options: ['cpp_std=c++17'],
    version: '0.1.2'
)

vma_subproject = subproject('vulkan-memory-allocator')
kom_subproject = subproject('kom')

vk = dependency('vulkan', version: '>=1.3.0', required: true)
vma = dependency('VulkanMemoryAllocator', version: '>=3.0.0', required: true)
kom = dependency('kom', version: '>=1.0.1', required: true)

if get_option('default_library') == 'shared'
  vkom_cpp_args = [ '-DVKOM_COMPILING_DYNAMIC_LIBRARY' ]
else
  vkom_cpp_args = []
endif

vkom_sources = [
    # public interface
    'src/vkom/platform.hpp',
    'src/vkom/interface.hpp',
    'src/vkom/enums.hpp',
    'src/vkom/dynlib.hpp',
    'src/vkom/object.hpp',
    'src/vkom/instance.hpp',
    'src/vkom/adapter.hpp',
    'src/vkom/device.hpp',
    'src/vkom/fence.hpp',
    'src/vkom/semaphore.hpp',
    'src/vkom/surface.hpp',
    'src/vkom/swapchain.hpp',
    'src/vkom/pipeline.hpp',
    'src/vkom/descriptor.hpp',
    'src/vkom/heap.hpp',
    'src/vkom/resource.hpp',
    'src/vkom/texture.hpp',
    'src/vkom/buffer.hpp',
    'src/vkom/queue.hpp',
    'src/vkom/cmdencoder.hpp',
    'src/vkom/cmdpasses.hpp',
    'src/vkom/cmdbatch.hpp',

    # internal helpers
    'src/vkom/internal/vulkan.hpp',
    'src/vkom/internal/enums.hpp',
    'src/vkom/internal/funcptrs.hpp',
    'src/vkom/internal/vksurface.hpp',
    'src/vkom/internal/vkdata.hpp',
    'src/vkom/internal/vma.hpp', 'src/vkom/internal/vma.cpp',

    # implementation
    'src/vkom/internal/object.hpp', 'src/vkom/internal/object.cpp',
    'src/vkom/internal/instance.hpp', 'src/vkom/internal/instance.cpp',
    'src/vkom/internal/adapter.hpp', 'src/vkom/internal/adapter.cpp',
    'src/vkom/internal/device.hpp', 'src/vkom/internal/device.cpp',
    'src/vkom/internal/fence.hpp', 'src/vkom/internal/fence.cpp',
    'src/vkom/internal/semaphore.hpp', 'src/vkom/internal/semaphore.cpp',
    'src/vkom/internal/surface.hpp', 'src/vkom/internal/surface.cpp',
    'src/vkom/internal/swapchain.hpp', 'src/vkom/internal/swapchain.cpp',
    'src/vkom/internal/pipeline.hpp', 'src/vkom/internal/pipeline.cpp',
    'src/vkom/internal/descriptor.hpp', 'src/vkom/internal/descriptor.cpp',
    'src/vkom/internal/heap.hpp', 'src/vkom/internal/heap.cpp',
    'src/vkom/internal/texture.hpp', 'src/vkom/internal/texture.cpp',
    'src/vkom/internal/buffer.hpp', 'src/vkom/internal/buffer.cpp',
    'src/vkom/internal/queue.hpp', 'src/vkom/internal/queue.cpp',
    'src/vkom/internal/cmdencoder.hpp', 'src/vkom/internal/cmdencoder.cpp',
    'src/vkom/internal/cmdpasses.hpp', 'src/vkom/internal/cmdpasses.cpp',
    'src/vkom/internal/cmdbatch.hpp', 'src/vkom/internal/cmdbatch.cpp',

    # platform-specific implementation
    'src/vkom/internal/nt/vksurface.cpp',
    'src/vkom/internal/nt/dynlib.hpp', 'src/vkom/internal/nt/dynlib.cpp',
    'src/vkom/internal/unix/vksurface.cpp',
    'src/vkom/internal/unix/dynlib.hpp', 'src/vkom/internal/unix/dynlib.cpp',
    'src/vkom/internal/apple/vksurface.cpp',
]

vkom_includes = include_directories('src')
vkom_dependencies = [ vk, vma, kom ]
vkom_override_options = [ 'cpp_std=c++17' ]

vkom_lib = library('vkom',
    sources: vkom_sources,
    include_directories: vkom_includes,
    dependencies: vkom_dependencies,
    override_options: vkom_override_options,
    cpp_args: vkom_cpp_args,
    install: true,
)

vkom_dep = declare_dependency(
    include_directories: vkom_includes,
    link_with: vkom_lib
)

meson.override_dependency('vkom', vkom_dep)
