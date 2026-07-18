project('vkom', 'cpp', default_options: ['cpp_std=c++17'])

vk = dependency('vulkan', version: '>=1.2.0', required: true)

vkom = static_library('vkom',
    sources: [
        'src/vkom/platform.hpp',
        'src/vkom/interface.hpp',
        'src/vkom/uuid.hpp',
        'src/vkom/enums.hpp',
        'src/vkom/dynlib.hpp', 'src/vkom/dynlib.cpp',
        'src/vkom/object.hpp', 'src/vkom/object.cpp',
        'src/vkom/instance.hpp', 'src/vkom/instance.cpp',
        'src/vkom/adapter.hpp',
        'src/vkom/device.hpp',
        'src/vkom/queue.hpp',
        'src/vkom/cmdlist.hpp',
        'src/vkom/internal/enums.hpp',
        'src/vkom/internal/funcptrs.hpp',
        'src/vkom/internal/instance.hpp', 'src/vkom/internal/instance.cpp',
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
