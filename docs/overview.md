# 1. Terminology

## 1.1. General Audio

- ***Sample***: a scalar value representing a point on a sound wave at a certain point in time (internally represented with a single IEEE-754 float)
- ***Channel***: a single stream of audio *samples*
- ***Channel** Configuration*: a description for what layout each *channel* number should map to (mono, left+right, mid+side, left+right+sub, ...)
- ***Frame***: a collection of *samples* that each correspond to their own *channel* given a certain *channel configuration*
- ***Cache***: a unit that can store a static or dynamic amount of *frames* which can be accessed later
- ***Grain***: a small grouping of *frames* at a given amount meant to represent a near-instant amount of audio to be produced, processed, cached, or played back

## 1.2. DAW/DSP

- ***Timestamp***: an integer value representing the number of *frames* since the start of rendering or playback
- ***Event***: a grouping of parameters with a specific *timestamp*
- ***Track***: a single timeline of audio *events* which can be dispatched to chosen *devices*

# 2. Devices

## 2.1. General Device Info

- All audio devices communicate with *grains*, however they can manage them differently internally (notably *caches*)
- All audio devices can accept *events*

## 2.2. Specific Device Categories

### 2.2.1. Producers

- Devices that generate *frames* from scratch with no inputs aside from configuration
  - Level 0 Synthesizers
- Devices that generate *frames* from external sources
  - Level 1 Synthesizers
  - Most virtual instruments

### 2.2.2. Processors

- Devices that take a pre-produced *grain* and modify it in some way
  - Effects Plug-ins
  - Compressors
  - Reverb

### 2.2.3. Cache

- Intermediate devices that can receive, send and store *frames*
  - Useful for implementing reverb and similar time-based *processors*

### 2.2.4. Render Targets

- Final output for audio (can either cache audio along time or only store the current grain size)
  - Speaker playback
  - Rendered audio file

### 2.2.5. Input Targets

- External sources that stream audio into the DAW
  - Microphones
  - Pre-existing audio files
  - Possibly recording other applications (though not planned)

### 2.2.6. Emitters

- Devices that can emit *events* for other devices to consume

### 2.2.7. Visualizers and Other Tools

- Devices that consume a stream of *grains* and provide some form of information
  - 

# 3. Events

## 3.1. General Structure of Events

- All events are exactly 64 bytes large
- First 8 bytes is always timestamp 0
  - This represents the start of the event
- Second 8 bytes is always timestamp 1
  - This represents the end of the event
- Third 4 bytes is always the event ID (used to determine the type of event)

## 3.2. Specific Events

### 3.2.1. Note Event

| range | type      | significance | value (if applicable) |
| ----- | --------- | ------------ | --------------------- |
| 0-8   | uint64_t  | timestamp 0  |                       |
| 8-16  | uint64_t  | timestamp 1  |                       |
| 16-20 | uint32_t  | id           | 0x00000001            |
| 20-24 | float32_t | frequency    |                       |
| 24-28 | float32_t | velocity     |                       |
| 28-64 | ...       | reserved     |                       |

### 3.2.2. Interpolation Event

| range | type      | significance | value (if applicable) |
| ----- | --------- | ------------ | --------------------- |
| 0-8   | uint64_t  | timestamp 0  |                       |
| 8-16  | uint64_t  | timestamp 1  |                       |
| 16-20 | uint32_t  | id           | 0x00000002            |
| 20-24 | float32_t | start        |                       |
| 24-28 | float32_t | end          |                       |
| 28-32 | uint32_t  | interpolator |                       |
| 32-36 | uint32_t  | effect       |                       |
| 36-64 | ...       | reserved     |                       |

# 4. What Work is Done By Who

## 4.1. Devices

- All devices function on the GPU, but might have CPU-sided elements
  - *Producers*, *processors* and *emitters* function as compute shaders which read and write to buffers
  - *Cache*, *render targets* and *input targets* are buffers that the GPU has access to
    - *Cache* is fully private to the GPU to increase performance
    - *Render targets* and *input targets* are CPU-accessible as playback, rendering to a file and recording is done through the CPU
  
## 4.2. Tracks and Events

### 4.2.1. CPU-side

- *Tracks* are a fully CPU-sided as they represent the order in which *devices* and *events* are dispatched
- *Events* that originate from a *track* are also fully CPU-sided until being dispatched

### 4.2.2 GPU-side

- Every *grain* that the DAW works with is GPU-side (with very few exceptions)
- While most *events* are dispatched from the CPU *tracks*, *events* can also be dispatched by *emitter* devices
  - These *events* are GPU-side for their entire lifetime

## 4.3. UI

- The UI is completely managed by the CPU and rendered using the GPU as typical with most other applications

# 5. Design Decisions

## 5.1. Software

### 5.1.1. CPU

- CPU-sided code should be the least interesting aspect of the code
  - Plain management of memory, synchronization, ...
  - Very few tricks or unstable solutions should be used
  - Should be dynamic and scalable
- Stick to a C ABI
- Avoid platform specificness
- Avoid unnecessary library dependencies
- Avoid forcing the user into a strict coding style (even though the user will mainly be the programmers of this project itself)
- When possible, make the software portable so that it could be used outside of the standalone DAW environment
- Hide implementation details, but make access possible, if requested by user code

### 5.1.2. GPU

- *Devices* that are represented with compute shaders should be given a firm and dynamic foundation to work with
  - 

## 5.2. Accepted Limitations and Device Support

- Devices lacking Vulkan 1.2 support
  - Unmaintained GPU devices/drivers
- Lack of typical DAW/DSP extensions (especially early on)
- Real-time rendering may have to utilize large *grains* (hopefully not more than one second at 44.1 kHz) to best benefit from parallelism