# Android Vulkan Renderer

High-performance Vulkan renderer for Android using NativeActivity. Renders the classic Viking Room 3D model with touch-based rotation controls.

## Features

- **Native Vulkan Rendering**: Direct Vulkan API 1.0 implementation for Android
- **High Performance**: Achieves 850-1050 FPS on 120Hz devices (Mali GPU)
- **NativeActivity**: Direct window access without GameActivity overhead
- **Touch Controls**: Intuitive drag-to-rotate with quaternion slerp interpolation (X & Z axes)
- **Mobile Optimized**: 
  - MSAA disabled for performance
  - Mipmaps disabled
  - Mailbox present mode for lowest latency
- **3D Model Loading**: OBJ model support via tiny_obj_loader
- **Texture Mapping**: STB image library for texture loading

## Tech Stack

### Android Platform (2026)
- **Android Gradle Plugin (AGP)**: 9.0.0 *(Latest stable)*
- **Compile SDK**: 36 (Android 16 Preview)
- **Target SDK**: 36
- **Min SDK**: 30 (Android 11)
- **NDK**: 27+ (bundled with Android Studio)
- **CMake**: 3.22.1
- **C++ Standard**: C++17
- **Kotlin**: 1.9+ (for MainActivity only)

### Graphics & Libraries
- **Vulkan API**: 1.0
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
- **Emulator**: Configure with host GPU mode for best performance (can achieve 3000+ FPS)

## Build Instructions

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd androidCpp
   ```

2. Compile shaders (if you modify them):
   ```bash
   cd app/src/main/assets
   glslc shader.vert -o shader.vert.spv
   glslc shader.frag -o shader.frag.spv
   ```
   *Note: Compiled shaders (.spv) are included, so this step is optional*

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
- **Touch Input**: Quaternion-based rotation with slerp smoothing

### Dependencies

- **GLM 1.0.3**: Header-only math library for matrix/quaternion operations
- **STB Image**: Single-header image loading library
- **tiny_obj_loader**: Lightweight OBJ model parser
- **android_native_app_glue**: NDK native activity support

## Configuration

Display settings in `MainActivity.kt`:
- Sustained performance mode enabled
- 120Hz refresh rate selection
- High refresh rate preference

## Performance

Tested on Xiaomi device with Mali GPU (MIUI):
- **FPS**: 850-1050 sustained
- **Display**: 120Hz (1220×2712)
- **Present Mode**: VK_PRESENT_MODE_MAILBOX_KHR

## License

This project is provided as-is for educational purposes. The Viking Room model and textures are from the Vulkan Tutorial and are CC0 licensed.
