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
    Camera(glm::vec3 position = glm::vec3(250.0f, 0.0f, 100.0f), glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f))
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

private:
    void updateViewMatrix() {
        glm::vec3 eye = position;
        glm::vec3 center = target;
        glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

        matrices.view = glm::lookAt(eye, center, up);

        // Apply device orientation rotation to compensate for swapchain transform
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

    struct {
        glm::mat4 perspective;
        glm::mat4 view;
    } matrices;
};