#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

enum class DeviceOrientation {
    Portrait0 = 0,
    Landscape90 = 1,
    Portrait180 = 2,
    Landscape270 = 3
};

class Camera {
public:
    Camera(glm::vec3 position = glm::vec3(-250.0f, 0.0f, 300.0f), glm::vec3 target = glm::vec3(0.0f, 0.0f, 80.0f))
            : target(target),
              position(position),
              currentOrientation(DeviceOrientation::Portrait0),
              aspectRatio(1.0f),
              fov(45.0f),
              nearPlane(0.1f),
              farPlane(910.0f),
              flipY(true),
              updated(false) {

        updateViewMatrix();
        updateProjectionMatrix();
    }

    void setOrientation(DeviceOrientation orientation) {
        if (currentOrientation != orientation) {
            currentOrientation = orientation;
            updateViewMatrix();
        }
    }

    void setAspectRatio(float width, float height) {
        float newAspect = width / height;
        // When camera is rotated 90/270 degrees (landscape), swap aspect ratio to match rotated view
        if (currentOrientation == DeviceOrientation::Landscape90 ||
            currentOrientation == DeviceOrientation::Landscape270) {
            newAspect = 1.0f / newAspect;
        }

        if (aspectRatio != newAspect) {
            aspectRatio = newAspect;
            updateProjectionMatrix();
        }
    }

    glm::mat4 getViewMatrix() const {
        return matrices.view;
    }

    glm::mat4 getProjectionMatrix() const {
        return matrices.perspective;
    }

    glm::vec3 getPosition() const {
        return position;
    }

    glm::vec3 getTarget() const {
        return target;
    }
    
    void setTarget(glm::vec3 newTarget) {
        target = newTarget;
        updateViewMatrix();
    }
    
    void setPosition(glm::vec3 newPos) {
        position = newPos;
        updateViewMatrix();
    }
    
    float getFov() const {
        return fov;
    }
    
    void setFov(float newFov) {
        fov = glm::clamp(newFov, 1.0f, 180.0f);
        updateProjectionMatrix();
    }

    void adjustTurntableRotation(float pitchDelta, float yawDelta) {
        targetTurntableRotation.x += pitchDelta;
        targetTurntableRotation.y += yawDelta;
        updateViewMatrix();
    }
    
    void updateTurntableDamping(float deltaTime) {
        // FPS-independent damping: interpolate current towards target
        float dampingFactor = 10.0f;
        currentTurntableRotation += (targetTurntableRotation - currentTurntableRotation) * dampingFactor * deltaTime;
        
        // Recalculate view matrix with smoothed rotation
        updateViewMatrix();
    }

private:
    void updateViewMatrix() {
        glm::vec3 eye = position;
        glm::vec3 center = target;
        glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
        
        // Use glm::lookAt to get base view matrix
        glm::mat4 lookAtMat = glm::lookAt(eye, center, up);
        
        // Build turntable rotation matrix from CURRENT rotation (smoothed)
        glm::mat4 turntableRotM = glm::mat4(1.0f);
        turntableRotM = glm::rotate(turntableRotM, currentTurntableRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        turntableRotM = glm::rotate(turntableRotM, currentTurntableRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        turntableRotM = glm::rotate(turntableRotM, currentTurntableRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        
        // Extract rotation part (3x3) from lookAt
        glm::mat4 lookAtRot = glm::mat3(lookAtMat);
        
        // Apply turntable rotation
        glm::mat4 combinedRot = turntableRotM * lookAtRot;
        
        // Rebuild view matrix: rotation part in upper 3x3, Translation in 4th column
        matrices.view = glm::mat4(combinedRot);
        matrices.view[3][0] = lookAtMat[3][0];
        matrices.view[3][1] = lookAtMat[3][1];
        matrices.view[3][2] = lookAtMat[3][2];
        
        // Apply device orientation rotation LAST
        float angle = 0.0f;
        switch (currentOrientation) {
            case DeviceOrientation::Landscape90:
                angle = glm::radians(-90.0f);
                break;
            case DeviceOrientation::Portrait180:
                angle = glm::radians(-180.0f);
                break;
            case DeviceOrientation::Landscape270:
                angle = glm::radians(-270.0f);
                break;
            default:
                break;
        }

        if (angle != 0.0f) {
            glm::mat4 preRotate = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
            matrices.view = preRotate * matrices.view;
        }

        updated = true;
    }

    void updateProjectionMatrix() {
        matrices.perspective = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
        if (flipY) {
            matrices.perspective[1][1] *= -1.0f;
        }
        updated = true;
    }

    glm::vec3 position;
    glm::vec3 target;

    DeviceOrientation currentOrientation;
    float aspectRatio;
    float fov;
    float nearPlane;
    float farPlane;
    bool flipY;
    bool updated;

    glm::vec3 turntableRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    
    glm::vec3 targetTurntableRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 currentTurntableRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    float dampingFactor = 10.0f;

    struct {
        glm::mat4 perspective;
        glm::mat4 view;
    } matrices;
};