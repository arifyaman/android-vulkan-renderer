#pragma once

#include <android_native_app_glue.h>
#include <jni.h>
#include "Camera.h"
#include "AndroidOut.h"

// Forward declaration
class VulkanRenderer;

// Track window resize state
struct WindowResizeState {
    bool contentRectChanged = false;
    bool redrawNeeded = false;
    
    void reset() {
        contentRectChanged = false;
        redrawNeeded = false;
    }
    
    bool shouldRecreateSwapchain() const {
        return contentRectChanged && redrawNeeded;
    }
};

class AndroidHelper {
public:
    // Get device orientation directly (combines rotation query and conversion)
    static DeviceOrientation getDeviceOrientation(android_app* app);

    // Get display rotation (0=0°, 1=90°, 2=180°, 3=270°)
    // Returns -1 on error
    static int32_t getDisplayRotation(android_app* app);

    // Convert Android rotation value to DeviceOrientation enum
    static DeviceOrientation rotationToOrientation(int32_t rotation);

    // Handle Android app commands
    static void handleCommand(android_app* app, int32_t cmd, WindowResizeState& resizeState);
};
