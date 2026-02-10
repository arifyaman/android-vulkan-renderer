#include <jni.h>
#include <android_native_app_glue.h>
#include "AndroidOut.h"
#include "VulkanRenderer.h"
#include "AndroidHelper.h"

extern "C"
{

    /*!
     * Handles touch input events
     */
    int32_t handle_input(android_app *app, AInputEvent *event)
    {
        auto *pRenderer = reinterpret_cast<VulkanRenderer *>(app->userData);
        if (pRenderer && AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
        {
            int32_t action = AMotionEvent_getAction(event);
            int32_t actionMasked = action & AMOTION_EVENT_ACTION_MASK;
            int32_t pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> 
                                  AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            int32_t pointerCount = AMotionEvent_getPointerCount(event);

            // Get pointer coordinates
            float x1 = AMotionEvent_getX(event, 0);
            float y1 = AMotionEvent_getY(event, 0);
            float x2 = (pointerCount >= 2) ? AMotionEvent_getX(event, 1) : 0.0f;
            float y2 = (pointerCount >= 2) ? AMotionEvent_getY(event, 1) : 0.0f;

            pRenderer->handleTouchInput(x1, y1, x2, y2, pointerCount, actionMasked);
            return 1;
        }
        return 0;
    }

    /*!
     * Handles commands sent to this Android application
     */
    static void handle_cmd(android_app *app, int32_t cmd)
    {
        AndroidHelper::handleCommand(app, cmd);
    }

    /*!
     * This is the main entry point for a native activity
     */
    void android_main(struct android_app *pApp)
    {
        aout << "Welcome to android_main (NativeActivity)" << std::endl;

        // Register event handlers
        pApp->onAppCmd = handle_cmd;
        pApp->onInputEvent = handle_input;

        // This sets up a typical game/event loop
        do
        {
            // Process all pending events before running game logic
            int events;
            android_poll_source *pSource;

            // Non-blocking poll (0 timeout) for maximum frame rate
            if (ALooper_pollOnce(0, nullptr, &events, reinterpret_cast<void **>(&pSource)) >= 0)
            {
                if (pSource)
                {
                    pSource->process(pApp, pSource);
                }
            }

            // Check if any user data is associated
            if (pApp->userData)
            {
                auto *pRenderer = reinterpret_cast<VulkanRenderer *>(pApp->userData);
                // Render a frame as fast as possible
                pRenderer->render();
            }
        } while (!pApp->destroyRequested);
    }
}