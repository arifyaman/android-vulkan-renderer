#include "AndroidHelper.h"
#include "VulkanRenderer.h"
#include <android/native_window.h>

DeviceOrientation AndroidHelper::getDeviceOrientation(android_app* app) {
    int32_t rotation = getDisplayRotation(app);
    return rotationToOrientation(rotation);
}

int32_t AndroidHelper::getDisplayRotation(android_app* app) {
    if (!app || !app->activity) return -1;
    
    JNIEnv* env;
    JavaVM* vm = app->activity->vm;
    if (vm->AttachCurrentThread(&env, nullptr) != JNI_OK) return -1;
    
    jobject activity = app->activity->clazz;
    jclass activityClass = env->GetObjectClass(activity);
    
    // Get WindowManager
    jmethodID getWindowManager = env->GetMethodID(activityClass, "getWindowManager", "()Landroid/view/WindowManager;");
    jobject windowManager = env->CallObjectMethod(activity, getWindowManager);
    
    // Get Display
    jclass windowManagerClass = env->GetObjectClass(windowManager);
    jmethodID getDefaultDisplay = env->GetMethodID(windowManagerClass, "getDefaultDisplay", "()Landroid/view/Display;");
    jobject display = env->CallObjectMethod(windowManager, getDefaultDisplay);
    
    // Get rotation
    jclass displayClass = env->GetObjectClass(display);
    jmethodID getRotation = env->GetMethodID(displayClass, "getRotation", "()I");
    int32_t rotation = env->CallIntMethod(display, getRotation);
    
    vm->DetachCurrentThread();
    
    return rotation;
}

DeviceOrientation AndroidHelper::rotationToOrientation(int32_t rotation) {
    switch (rotation) {
        case 0:  return DeviceOrientation::Portrait0;
        case 1:  return DeviceOrientation::Landscape90;
        case 2:  return DeviceOrientation::Portrait180;
        case 3:  return DeviceOrientation::Landscape270;
        default: return DeviceOrientation::Portrait0;
    }
}

void AndroidHelper::handleCommand(android_app* app, int32_t cmd, WindowResizeState& resizeState) {
    auto* renderer = reinterpret_cast<VulkanRenderer*>(app->userData);
    
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (app->window != nullptr) {
                aout << "APP_CMD_INIT_WINDOW" << std::endl;
                if (renderer == nullptr) {
                    renderer = new VulkanRenderer(app);
                    app->userData = renderer;
                }
                resizeState.reset();
            }
            break;
            
        case APP_CMD_TERM_WINDOW:
            aout << "APP_CMD_TERM_WINDOW" << std::endl;
            if (renderer != nullptr) {
                delete renderer;
                app->userData = nullptr;
            }
            resizeState.reset();
            break;
            
        case APP_CMD_CONTENT_RECT_CHANGED:
            {
                int32_t width = ANativeWindow_getWidth(app->window);
                int32_t height = ANativeWindow_getHeight(app->window);
                aout << "APP_CMD_CONTENT_RECT_CHANGED - Window: " << width << "x" << height << std::endl;
                resizeState.contentRectChanged = true;
            }
            if (renderer && resizeState.shouldRecreateSwapchain()) {
                aout << "Both conditions met - recreating swapchain" << std::endl;
                renderer->recreateSwapChain();
                resizeState.reset();
            }
            break;
            
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            {
                int32_t width = ANativeWindow_getWidth(app->window);
                int32_t height = ANativeWindow_getHeight(app->window);
                aout << "APP_CMD_WINDOW_REDRAW_NEEDED - Window: " << width << "x" << height << std::endl;
                resizeState.redrawNeeded = true;
            }
            if (renderer && resizeState.shouldRecreateSwapchain()) {
                aout << "Both conditions met - recreating swapchain" << std::endl;
                renderer->recreateSwapChain();
                resizeState.reset();
            }
            break;
    }
}
