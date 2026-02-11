# Android Vulkan Renderer

High-performance Vulkan renderer for Android using NativeActivity. Renders high-polygon 3D models with multi-touch camera controls.

![Image Strip Example](demo/demo.png)

## Features

- **Native Vulkan Rendering**: Direct Vulkan API 1.0 implementation for Android
- **High Performance**: Achieves 850-1050 FPS on 120Hz devices (Mali GPU)
- **NativeActivity**: Direct window access without GameActivity overhead
- **Multi-Touch Controls**:
    - **Drag**: Rotate camera with quaternion slerp interpolation (X & Z axes)
    - **Pinch**: Zoom in/out
    - **Pan (2-finger)**: Translate camera position
- **Mobile Optimized**:
    - MSAA disabled for performance
    - Mipmaps disabled
    - Mailbox present mode for lowest latency
- **3D Model Loading**: OBJ model support via tiny_obj_loader
- **Texture Mapping**: STB image library for texture loading
- **High Poly Support**: Renders complex models (50k triangles / 25k vertices)

## Current Model

**Default**: `logo.obj`
- Triangles: 50,000
- Vertices: 25,000
- Rendered as single mesh object

**Alternative**: `AGirl.obj`
- High-detail character model
- CC Attribution License
- Creator: [Kensyouen](https://sketchfab.com/Kensyouen)
- Source: [Sketchfab - Just a Girl](https://sketchfab.com/3d-models/just-a-girl-b2359160a4f54e76b5ae427a55d9594d)

**Alternative**: `viking_room.obj`
- From vulkan tutorials
- [CC BY 4.0](https://web.archive.org/web/20200428202538/https://sketchfab.com/3d-models/viking-room-a49f1b8e4f5c4ecf9e1fe7d81915ad38)
- Creator: [nigelgoh](https://sketchfab.com/nigelgoh)
- Source: [Sketchfab - Viking room](https://sketchfab.com/3d-models/viking-room-a49f1b8e4f5c4ecf9e1fe7d81915ad38)

To switch models, update the filename in `VulkanRenderer.cpp` (```loadModel``` method).

## Tech Stack

### Android Platform (2026)
- **Android Gradle Plugin (AGP)**: 9.0.0 *(Latest stable)*
- **Compile SDK**: 36 (Android 16 Preview)
- **Target SDK**: 36
- **Min SDK**: 24 (Android 7.0)
- **NDK**: 27+ (bundled with Android Studio)
- **CMake**: 3.22.1
- **C++ Standard**: C++17
- **Kotlin**: 1.9+ (for MainActivity only)

### Graphics & Libraries
- **Vulkan API**: 1.0
- **Slang Shader Compiler**: Latest (unified shader language)
- **GLM**: 1.0.3 (header-only math library)
- **STB Image**: Latest (single-header image loader)
- **tiny_obj_loader**: Latest (OBJ model parser)

### Build System
- **Gradle**: 8.9+
- **Java**: 11 (target/source compatibility)
- **AndroidX**: Enabled

## Requirements

- **Android Studio**: Ladybug (2024.2.1) or newer
- **JDK**: 17 or higher (for Gradle 8.9+)
- **Vulkan Device**: Android 7.0+ (API 24+) with Vulkan support
- **Slang Compiler**: Optional, for shader compilation (https://shader-slang.com/)
- **Emulator**: Configure with host GPU mode for best performance (can achieve 3000+ FPS)

## Build Instructions

1. Clone the repository:
```bash
   git clone <repository-url>
   cd androidCpp
```

2. Compile shaders:

   **Option A: Using Slang (Recommended - Single SPIR-V file)**
```bash
   cd app/src/main/assets
   slangc shader.slang -target spirv -entry vertexMain -stage vertex -entry fragmentMain -stage fragment -o shader.spv
```
Set `useCombinedSPIRV = true` in `VulkanRenderer.h` (default)

**Option B: Using GLSL (Separate shader files)**
```bash
   cd app/src/main/assets
   glslc shader.vert -o shader.vert.spv
   glslc shader.frag -o shader.frag.spv
```
Set `useCombinedSPIRV = false` in `VulkanRenderer.h`

*Note: Compiled shaders are included, so this step is optional unless modifying shaders*

3. Open project in Android Studio or build via command line:
```bash
   ./gradlew assembleDebug
```

4. Install on device:
```bash
   ./gradlew installDebug
```

## Architecture

- **Renderer**: `VulkanRenderer.cpp` - Complete Vulkan rendering pipeline
- **Entry Point**: `main.cpp` - NativeActivity initialization and event loop
- **Touch Input**: `CameraController.cpp` Multi-touch gesture recognition with quaternion-based rotation, pinch-to-zoom, and 2-finger pan

### Dependencies

- **GLM 1.0.3**: Header-only math library for matrix/quaternion operations
- **STB Image**: Single-header image loading library
- **tiny_obj_loader**: Lightweight OBJ model parser
- **android_native_app_glue**: NDK native activity support
  
(All are included in the project)
## Configuration

Display settings in `MainActivity.kt`:
- Sustained performance mode enabled
- 120Hz refresh rate selection
- High refresh rate preference

## Branches

- **main**: Standard Vulkan 1.0 rendering pipeline
- **dynamic-rendering**: Uses VK_KHR_dynamic_rendering extension for modern Vulkan rendering (eliminates render passes and framebuffers)

## Performance
With viking room
- **Present Mode**: VK_PRESENT_MODE_MAILBOX_KHR

Tested on Xiaomi device with Mali GPU (MIUI):
- **FPS**: 850-1050 sustained
- **Display**: 120Hz (1220×2712)


Tested on Xiaomi Redmi Note 4 (API 24) Qualcomm Adreno:
- **FPS**: 300 sustained
- **Configuration**: `useCombinedSPIRV = false`
