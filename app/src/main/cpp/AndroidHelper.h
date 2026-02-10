#pragma once

#include <android_native_app_glue.h>
#include <jni.h>
#include "Camera.h"
#include "AndroidOut.h"
#include "VulkanRenderer.h"

class AndroidHelper {
public:
    // Handle Android app commands
    static void handleCommand(android_app* app, int32_t cmd);
};
