project('vkom', 'cpp', default_options: ['cpp_std=c++17'])

vk = dependency('vulkan', version: '>=1.2.0', required: true)

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
        'src/vkom/queue.hpp', 'src/vkom/queue.cpp',
        'src/vkom/cmdlist.hpp', 'src/vkom/cmdlist.cpp',
        'src/vkom/internal/vulkan.hpp',
        'src/vkom/internal/enums.hpp',
        'src/vkom/internal/funcptrs.hpp',
        'src/vkom/internal/instance.hpp', 'src/vkom/internal/instance.cpp',
        'src/vkom/internal/adapter.hpp', 'src/vkom/internal/adapter.cpp',
        'src/vkom/internal/device.hpp', 'src/vkom/internal/device.cpp',
        'src/vkom/internal/queue.hpp', 'src/vkom/internal/queue.cpp',
        'src/vkom/internal/cmdlist.hpp', 'src/vkom/internal/cmdlist.cpp',
        'src/vkom/internal/nt/dynlib.hpp', 'src/vkom/internal/nt/dynlib.cpp',
        'src/vkom/internal/unix/dynlib.hpp', 'src/vkom/internal/unix/dynlib.cpp',
    ],
    include_directories: include_directories('src'),
    dependencies: [ vk ],
    override_options: ['cpp_std=c++17'],
    install: true,
)

executable('vkom_test',
    sources: [
        'src/test.cpp',
    ],
    include_directories: include_directories('src'),
    override_options: ['cpp_std=c++17'],
    link_with: [ vkom ],
)
