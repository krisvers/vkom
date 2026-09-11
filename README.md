# vkom

[COM](https://en.wikipedia.org/wiki/Component_Object_Model)-style C++17 library wrapping around [Vulkan](https://vulkan.org) ([API documentation](https://docs.vulkan.org/spec/latest/index.html))

- NOTE: no AI tooling was used to create any of this project whether it be assets, code, comments, configuration, ...; everything was painfully and lovingly hand-written

## design

- basically just inherited from core Vulkan 1.x design plus less stressful methods for accessing extensions

### notable changes

- `VkImage` corresponds to `vkom::ITexture`
- inclusion of base contract interfaces like `vkom::IResource`
- type restrictions on methods/commands using resources (i.e. `vkom::ICommandEncoder::copyBufferToBuffer` requires the destination to be of type `vkom::ITransferDestinationBuffer` and similarly for source ...)
- addition of `vkom::IBackbuffer` (which inherits from `vkom::ITexture`, ...) for help managing swapchain images

## COM-style behavior

- inherited from [krisvers/kom](https://github.com/krisvers/kom) (as this is the source of the [COM](https://en.wikipedia.org/wiki/Component_Object_Model)-style behavior)
- generally:
  - interfaces are a window into an object
  - the interfaces always share a single object
  - very flattened inheritance (using pure virtuals and virtual inheritance so that conceptually there's no real inheritance but more of a contract-like system)
  - query an interface's support (which can be dynamically determined like with `vkom::IDevice` implementations deciding whether to advertise support for `vkom::IWSIDevice` based on Vulkan device capabilities)

## project structure

- `src/vkom/...`: interface declarations and general contracts exposed to the user
  - for example: `src/vkom/adapter.hpp` declares a few structs (`vkom::AdapterInfo`, `vkom::AdapterFeatures`, `vkom::AdapterLimits`) and the interface `vkom::IAdapter` which can be used for instantiating `vkom::IDevice` instances
- `src/vkom/internal/...`: internal Vulkan/platform-agnostic implementations of interfaces and their contracts
  - for example: `src/vkom/internal/adapter.hpp` declares a Vulkan implementation of `vkom::IAdapter` (implemented as `vkom::internal::VulkanAdapter`) and implements behavior in `src/vkom/internal/adapter.cpp`
- `src/vkom/internal/[platform]/...`: internal [platform]-specific implementations of ...
  - for example: similarly to the `adapter.cpp`/`adapter.hpp` pair inside of `src/vkom/internal/`, there is a pair for `dynlib.cpp`/`dynlib.hpp` inside of `src/vkom/internal/nt/` and `src/vkom/internal/unix/`
  - another example would be the `vksurface.cpp` helpers in each platform's folder
