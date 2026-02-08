#include "CameraController.h"
#include "AndroidOut.h"
#include <algorithm>
#include <cmath>

CameraController::CameraController(Camera& camera)
    : camera(camera)
    , isDragging(false)
    , lastTouchX(0.0f)
    , lastTouchY(0.0f)
    , lastTwoFingerDistance(0.0f)
    , lastTwoFingerMidpoint(0.0f)
    , twoFingerStartCameraPos(0.0f)
    , twoFingerStartTargetPos(0.0f)
    , minDistance(10.0f)
    , maxDistance(1000.0f)
    , rotationSensitivity(3.0f)
    , panSensitivityMultiplier(2.0f)
    , zoomThreshold(1.0f)
    , panThreshold(1.0f)
{
}

void CameraController::handleTouchInput(float x1, float y1, float x2, float y2,
                                        int pointerCount, int32_t actionMasked) {
    float maxDim = std::max(screenWidth, screenHeight);

    if (pointerCount == 2) {
        handleTwoFingerGesture(x1, y1, x2, y2, actionMasked, maxDim);
        // Prevent single-finger drag from interfering
        isDragging = false;
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
        lastTwoFingerDistance = currentDistance;
        lastTwoFingerMidpoint = currentMidpoint;
        twoFingerStartCameraPos = camera.getPosition();
        twoFingerStartTargetPos = camera.getTarget();

        aout << "Two-finger gesture started" << std::endl;
    }
    else if (actionMasked == AMOTION_EVENT_ACTION_MOVE) {
        // Calculate deltas
        float distanceDelta = currentDistance - lastTwoFingerDistance;
        glm::vec2 midpointDelta = currentMidpoint - lastTwoFingerMidpoint;

        // Handle pinch zoom
        if (std::abs(distanceDelta) > zoomThreshold) {
            handlePinchZoom(currentDistance, distanceDelta);
        }

        // Handle pan
        float midpointMovement = glm::length(midpointDelta);
        if (midpointMovement > panThreshold) {
            handlePan(midpointDelta, maxDim);
        }

        // Update last values for next frame
        lastTwoFingerDistance = currentDistance;
        lastTwoFingerMidpoint = currentMidpoint;
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
        isDragging = true;
        lastTouchX = x1;
        lastTouchY = y1;
        aout << "Single-finger drag started" << std::endl;
    }
    else if (actionMasked == AMOTION_EVENT_ACTION_MOVE && isDragging) {
        // Calculate delta from last position
        float deltaX = x1 - lastTouchX;
        float deltaY = y1 - lastTouchY;

        // Normalize to screen size
        float normalizedDeltaX = deltaX / maxDim;
        float normalizedDeltaY = deltaY / maxDim;

        // Apply rotation sensitivity
        float yawDelta = normalizedDeltaX * rotationSensitivity;
        float pitchDelta = normalizedDeltaY * rotationSensitivity;

        // Apply turntable rotation (orbit around target)
        camera.adjustTurntableRotation(pitchDelta, yawDelta);

        aout << "Rotating - yaw: " << yawDelta << " pitch: " << pitchDelta << std::endl;

        // Update last position
        lastTouchX = x1;
        lastTouchY = y1;
    }
    else if (actionMasked == AMOTION_EVENT_ACTION_UP ||
             actionMasked == AMOTION_EVENT_ACTION_CANCEL) {
        isDragging = false;
        aout << "Single-finger drag ended" << std::endl;
    }
}

void CameraController::handlePinchZoom(float currentDistance, float distanceDelta) {
    float zoomFactor = currentDistance / lastTwoFingerDistance;

    // Calculate new camera distance from target
    glm::vec3 camToTarget = camera.getTarget() - camera.getPosition();
    float currentCamDistance = glm::length(camToTarget);
    float newCamDistance = currentCamDistance / zoomFactor;

    // Clamp distance
    newCamDistance = glm::clamp(newCamDistance, minDistance, maxDistance);

    // Move camera along view direction
    glm::vec3 viewDir = glm::normalize(camToTarget);
    glm::vec3 newCameraPos = camera.getTarget() - viewDir * newCamDistance;

    camera.setPosition(newCameraPos);

    aout << "Pinch zoom - factor: " << zoomFactor << " distance: " << newCamDistance << std::endl;
}

void CameraController::handlePan(const glm::vec2& midpointDelta, float maxDim) {
    // Normalize the movement
    float normalizedDeltaX = midpointDelta.x / maxDim;
    float normalizedDeltaY = midpointDelta.y / maxDim;

    // Calculate camera coordinate system
    glm::vec3 viewDir = glm::normalize(camera.getTarget() - camera.getPosition());
    glm::vec3 worldUp(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(viewDir, worldUp));
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, viewDir));

    // Pan sensitivity scales with distance from target
    float camDistance = glm::length(camera.getTarget() - camera.getPosition());
    float panSensitivity = camDistance * panSensitivityMultiplier;

    // Calculate movement in world space (inverted X for natural feel)
    glm::vec3 panMovement = -cameraRight * (normalizedDeltaX * panSensitivity)
                           + cameraUp * (normalizedDeltaY * panSensitivity);

    // Move both camera and target together
    camera.setPosition(camera.getPosition() + panMovement);
    camera.setTarget(camera.getTarget() + panMovement);

    aout << "Pan - movement: (" << panMovement.x << ", " << panMovement.y << ", " << panMovement.z << ")" << std::endl;
}

void CameraController::setDistanceLimits(float min, float max) {
    minDistance = min;
    maxDistance = max;
}

void CameraController::setRotationSensitivity(float sensitivity) {
    rotationSensitivity = sensitivity;
}

void CameraController::setPanSensitivity(float sensitivity) {
    panSensitivityMultiplier = sensitivity;
}

void CameraController::setZoomThreshold(float threshold) {
    zoomThreshold = threshold;
}

void CameraController::setPanThreshold(float threshold) {
    panThreshold = threshold;
}

void CameraController::reset() {
    isDragging = false;
    lastTouchX = 0.0f;
    lastTouchY = 0.0f;
    lastTwoFingerDistance = 0.0f;
    lastTwoFingerMidpoint = glm::vec2(0.0f);
    twoFingerStartCameraPos = glm::vec3(0.0f);
    twoFingerStartTargetPos = glm::vec3(0.0f);
}
