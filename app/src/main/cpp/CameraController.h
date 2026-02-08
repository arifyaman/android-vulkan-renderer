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
    
    // Main touch input handler
    void handleTouchInput(float x1, float y1, float x2, float y2, 
                         int pointerCount, int32_t actionMasked,
                         int32_t screenWidth, int32_t screenHeight);
    
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
    Camera& camera_;
    
    // Touch input state
    bool isDragging_;
    float lastTouchX_;
    float lastTouchY_;
    float lastTwoFingerDistance_;
    glm::vec2 lastTwoFingerMidpoint_;
    glm::vec3 twoFingerStartCameraPos_;
    glm::vec3 twoFingerStartTargetPos_;
    
    // Configuration
    float minDistance_;
    float maxDistance_;
    float rotationSensitivity_;
    float panSensitivityMultiplier_;
    float zoomThreshold_;
    float panThreshold_;
    
    // Helper methods
    void handleTwoFingerGesture(float x1, float y1, float x2, float y2,
                               int32_t actionMasked, float maxDim);
    void handleSingleFingerGesture(float x1, float y1,
                                  int32_t actionMasked, float maxDim);
    void handlePinchZoom(float currentDistance, float distanceDelta);
    void handlePan(const glm::vec2& midpointDelta, float maxDim);
};
