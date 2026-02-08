#pragma once

#include <glm/glm.hpp>
#include <android/input.h>
#include <android/log.h>
#include <android/native_window.h>
#include "Camera.h"
#include <memory>

// Android input event action flags
#define AMOTION_EVENT_ACTION_DOWN 0
#define AMOTION_EVENT_ACTION_UP 1
#define AMOTION_EVENT_ACTION_MOVE 2
#define AMOTION_EVENT_ACTION_CANCEL 3
#define AMOTION_EVENT_ACTION_POINTER_DOWN 5
#define AMOTION_EVENT_ACTION_POINTER_UP 6

class CameraController {
public:
    CameraController(Camera& camera);

    void setScreenDimensions(int32_t width, int32_t height) {
        screenWidth = width;
        screenHeight = height;
    }

    // Main touch input handler
    void handleTouchInput(float x1, float y1, float x2, float y2, 
                         int pointerCount, int32_t actionMasked);
    
    // Configuration
    void setDistanceLimits(float min, float max);
    void setRotationSensitivity(float sensitivity);
    void setPanSensitivity(float sensitivity);
    void setZoomThreshold(float threshold);
    void setPanThreshold(float threshold);
    
    // Reset state
    void reset();

private:
    // Reference to camera (not owned)
    Camera& camera;

    int32_t screenWidth;
    int32_t screenHeight;
    
    // Touch input state
    bool isDragging;
    float lastTouchX;
    float lastTouchY;
    float lastTwoFingerDistance;
    glm::vec2 lastTwoFingerMidpoint;
    glm::vec3 twoFingerStartCameraPos;
    glm::vec3 twoFingerStartTargetPos;
    
    // Configuration
    float minDistance;
    float maxDistance;
    float rotationSensitivity;
    float panSensitivityMultiplier;
    float zoomThreshold;
    float panThreshold;
    
    // Helper methods
    void handleTwoFingerGesture(float x1, float y1, float x2, float y2,
                               int32_t actionMasked, float maxDim);
    void handleSingleFingerGesture(float x1, float y1,
                                  int32_t actionMasked, float maxDim);
    void handlePinchZoom(float currentDistance, float distanceDelta);
    void handlePan(const glm::vec2& midpointDelta, float maxDim);
};
