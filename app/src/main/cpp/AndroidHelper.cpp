#include "AndroidHelper.h"
#include "VulkanRenderer.h"
#include <android/native_window.h>

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
