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
    // Handle Android app commands
    static void handleCommand(android_app* app, int32_t cmd, WindowResizeState& resizeState);
};
