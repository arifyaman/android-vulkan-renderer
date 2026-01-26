#include <jni.h>
#include <android_native_app_glue.h>
#include "AndroidOut.h"
#include "VulkanRenderer.h"

extern "C" {

/*!
 * Handles touch input events
 */
int32_t handle_input(android_app* app, AInputEvent* event) {
    auto* pRenderer = reinterpret_cast<VulkanRenderer*>(app->userData);
    if (pRenderer && AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        int32_t action = AMotionEvent_getAction(event);
        int32_t actionMasked = action & AMOTION_EVENT_ACTION_MASK;
        
        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);
        
        if (actionMasked == AMOTION_EVENT_ACTION_DOWN || 
            actionMasked == AMOTION_EVENT_ACTION_MOVE) {
            pRenderer->handleTouchInput(x, y, true);
            return 1;
        } else if (actionMasked == AMOTION_EVENT_ACTION_UP ||
                   actionMasked == AMOTION_EVENT_ACTION_CANCEL) {
            pRenderer->handleTouchInput(x, y, false);
            return 1;
        }
    }
    return 0;
}

/*!
 * Handles commands sent to this Android application
 * @param pApp the app the commands are coming from
 * @param cmd the command to handle
 */
void handle_cmd(android_app *pApp, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            // A new window is created, associate a renderer with it
            pApp->userData = new VulkanRenderer(pApp);
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being destroyed, clean up
            if (pApp->userData) {
                auto *pRenderer = reinterpret_cast<VulkanRenderer *>(pApp->userData);
                pApp->userData = nullptr;
                delete pRenderer;
            }
            break;
        default:
            break;
    }
}

/*!
 * This is the main entry point for a native activity
 */
void android_main(struct android_app *pApp) {
    aout << "Welcome to android_main (NativeActivity)" << std::endl;

    // Register event handlers
    pApp->onAppCmd = handle_cmd;
    pApp->onInputEvent = handle_input;

    // This sets up a typical game/event loop
    do {
        // Process all pending events before running game logic
        int events;
        android_poll_source *pSource;
        
        // Non-blocking poll (0 timeout) for maximum frame rate
        if (ALooper_pollOnce(0, nullptr, &events, reinterpret_cast<void**>(&pSource)) >= 0) {
            if (pSource) {
                pSource->process(pApp, pSource);
            }
        }

        // Check if any user data is associated
        if (pApp->userData) {
            auto *pRenderer = reinterpret_cast<VulkanRenderer *>(pApp->userData);
            // Render a frame as fast as possible
            pRenderer->render();
        }
    } while (!pApp->destroyRequested);
}
}