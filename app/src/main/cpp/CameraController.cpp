#include "CameraController.h"
#include "AndroidOut.h"
#include <algorithm>
#include <cmath>

CameraController::CameraController(Camera& camera)
    : camera_(camera)
    , isDragging_(false)
    , lastTouchX_(0.0f)
    , lastTouchY_(0.0f)
    , lastTwoFingerDistance_(0.0f)
    , lastTwoFingerMidpoint_(0.0f)
    , twoFingerStartCameraPos_(0.0f)
    , twoFingerStartTargetPos_(0.0f)
    , minDistance_(10.0f)
    , maxDistance_(1000.0f)
    , rotationSensitivity_(3.0f)
    , panSensitivityMultiplier_(2.0f)
    , zoomThreshold_(1.0f)
    , panThreshold_(1.0f)
{
}

void CameraController::handleTouchInput(float x1, float y1, float x2, float y2,
                                        int pointerCount, int32_t actionMasked,
                                        int32_t screenWidth, int32_t screenHeight) {
    float maxDim = std::max(screenWidth, screenHeight);
    
    if (pointerCount == 2) {
        handleTwoFingerGesture(x1, y1, x2, y2, actionMasked, maxDim);
        // Prevent single-finger drag from interfering
        isDragging_ = false;
    } else if (pointerCount == 1) {
        handleSingleFingerGesture(x1, y1, actionMasked, maxDim);
    }
}

void CameraController::handleTwoFingerGesture(float x1, float y1, float x2, float y2,
                                              int32_t actionMasked, float maxDim) {
    // Calculate current pinch distance and midpoint
    float dx = x2 - x1;
    float dy = y2 - y1;
    float currentDistance = std::sqrt(dx * dx + dy * dy);
    glm::vec2 currentMidpoint(((x1 + x2) * 0.5f), (y1 + y2) * 0.5f);
    
    if (actionMasked == AMOTION_EVENT_ACTION_POINTER_DOWN) {
        // Initialize two-finger gesture tracking
        lastTwoFingerDistance_ = currentDistance;
        lastTwoFingerMidpoint_ = currentMidpoint;
        twoFingerStartCameraPos_ = camera_.getPosition();
        twoFingerStartTargetPos_ = camera_.getTarget();
        
        aout << "Two-finger gesture started" << std::endl;
    }
    else if (actionMasked == AMOTION_EVENT_ACTION_MOVE) {
        // Calculate deltas
        float distanceDelta = currentDistance - lastTwoFingerDistance_;
        glm::vec2 midpointDelta = currentMidpoint - lastTwoFingerMidpoint_;
        
        // Handle pinch zoom
        if (std::abs(distanceDelta) > zoomThreshold_) {
            handlePinchZoom(currentDistance, distanceDelta);
        }
        
        // Handle pan
        float midpointMovement = glm::length(midpointDelta);
        if (midpointMovement > panThreshold_) {
            handlePan(midpointDelta, maxDim);
        }
        
        // Update last values for next frame
        lastTwoFingerDistance_ = currentDistance;
        lastTwoFingerMidpoint_ = currentMidpoint;
    }
    else if (actionMasked == AMOTION_EVENT_ACTION_POINTER_UP ||
             actionMasked == AMOTION_EVENT_ACTION_UP ||
             actionMasked == AMOTION_EVENT_ACTION_CANCEL) {
        aout << "Two-finger gesture ended" << std::endl;
    }
}

void CameraController::handleSingleFingerGesture(float x1, float y1,
                                                 int32_t actionMasked, float maxDim) {
    if (actionMasked == AMOTION_EVENT_ACTION_DOWN) {
        isDragging_ = true;
        lastTouchX_ = x1;
        lastTouchY_ = y1;
        aout << "Single-finger drag started" << std::endl;
    }
    else if (actionMasked == AMOTION_EVENT_ACTION_MOVE && isDragging_) {
        // Calculate delta from last position
        float deltaX = x1 - lastTouchX_;
        float deltaY = y1 - lastTouchY_;
        
        // Normalize to screen size
        float normalizedDeltaX = deltaX / maxDim;
        float normalizedDeltaY = deltaY / maxDim;
        
        // Apply rotation sensitivity
        float yawDelta = normalizedDeltaX * rotationSensitivity_;
        float pitchDelta = normalizedDeltaY * rotationSensitivity_;
        
        // Apply turntable rotation (orbit around target)
        camera_.adjustTurntableRotation(pitchDelta, yawDelta);
        
        aout << "Rotating - yaw: " << yawDelta << " pitch: " << pitchDelta << std::endl;
        
        // Update last position
        lastTouchX_ = x1;
        lastTouchY_ = y1;
    }
    else if (actionMasked == AMOTION_EVENT_ACTION_UP ||
             actionMasked == AMOTION_EVENT_ACTION_CANCEL) {
        isDragging_ = false;
        aout << "Single-finger drag ended" << std::endl;
    }
}

void CameraController::handlePinchZoom(float currentDistance, float distanceDelta) {
    float zoomFactor = currentDistance / lastTwoFingerDistance_;
    
    // Calculate new camera distance from target
    glm::vec3 camToTarget = camera_.getTarget() - camera_.getPosition();
    float currentCamDistance = glm::length(camToTarget);
    float newCamDistance = currentCamDistance / zoomFactor;
    
    // Clamp distance
    newCamDistance = glm::clamp(newCamDistance, minDistance_, maxDistance_);
    
    // Move camera along view direction
    glm::vec3 viewDir = glm::normalize(camToTarget);
    glm::vec3 newCameraPos = camera_.getTarget() - viewDir * newCamDistance;
    
    camera_.setPosition(newCameraPos);
    
    aout << "Pinch zoom - factor: " << zoomFactor << " distance: " << newCamDistance << std::endl;
}

void CameraController::handlePan(const glm::vec2& midpointDelta, float maxDim) {
    // Normalize the movement
    float normalizedDeltaX = midpointDelta.x / maxDim;
    float normalizedDeltaY = midpointDelta.y / maxDim;
    
    // Calculate camera coordinate system
    glm::vec3 viewDir = glm::normalize(camera_.getTarget() - camera_.getPosition());
    glm::vec3 worldUp(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(viewDir, worldUp));
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, viewDir));
    
    // Pan sensitivity scales with distance from target
    float camDistance = glm::length(camera_.getTarget() - camera_.getPosition());
    float panSensitivity = camDistance * panSensitivityMultiplier_;
    
    // Calculate movement in world space (inverted X for natural feel)
    glm::vec3 panMovement = -cameraRight * (normalizedDeltaX * panSensitivity)
                           + cameraUp * (normalizedDeltaY * panSensitivity);
    
    // Move both camera and target together
    camera_.setPosition(camera_.getPosition() + panMovement);
    camera_.setTarget(camera_.getTarget() + panMovement);
    
    aout << "Pan - movement: (" << panMovement.x << ", " << panMovement.y << ", " << panMovement.z << ")" << std::endl;
}

void CameraController::setDistanceLimits(float min, float max) {
    minDistance_ = min;
    maxDistance_ = max;
}

void CameraController::setRotationSensitivity(float sensitivity) {
    rotationSensitivity_ = sensitivity;
}

void CameraController::setPanSensitivity(float sensitivity) {
    panSensitivityMultiplier_ = sensitivity;
}

void CameraController::setZoomThreshold(float threshold) {
    zoomThreshold_ = threshold;
}

void CameraController::setPanThreshold(float threshold) {
    panThreshold_ = threshold;
}

void CameraController::reset() {
    isDragging_ = false;
    lastTouchX_ = 0.0f;
    lastTouchY_ = 0.0f;
    lastTwoFingerDistance_ = 0.0f;
    lastTwoFingerMidpoint_ = glm::vec2(0.0f);
    twoFingerStartCameraPos_ = glm::vec3(0.0f);
    twoFingerStartTargetPos_ = glm::vec3(0.0f);
}
