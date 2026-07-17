project('vkom', 'cpp', default_options: ['cpp_std=c++17'])

vk = dependency('vulkan', version: '>=1.2.0', required: true)

vkom = static_library('vkom',
    sources: [
        'src/vkom/interface.hpp',
        'src/vkom/uuid.hpp',
        'src/vkom/object.hpp', 'src/vkom/object.cpp',
        'src/vkom/instance.hpp', 'src/vkom/instance.cpp',
        'src/vkom/device.hpp', 'src/vkom/device.cpp',
        'src/vkom/queue.hpp', 'src/vkom/queue.cpp',
        'src/vkom/cmdlist.hpp', 'src/vkom/cmdlist.cpp',
    ],
    include_directories: include_directories('src'),
    dependencies: [ vk ],
    override_options: ['cpp_std=c++17'],
    install: true,
)

sdl3 = dependency('sdl3', version: '>=3.0.0', required: true)

executable('vkom_test',
    sources: [
        'src/test.cpp',
    ],
    include_directories: include_directories('src'),
    dependencies: [ sdl3 ],
    override_options: ['cpp_std=c++17'],
    link_with: [ vkom ],
)
