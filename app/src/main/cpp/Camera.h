#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class DeviceOrientation {
    Portrait0 = 0,    // 0° - Natural orientation (portrait)
    Landscape90 = 1,  // 90° - Rotated left
    Portrait180 = 2,  // 180° - Upside down portrait
    Landscape270 = 3  // 270° - Rotated right
};

class Camera {
   public:
    Camera(glm::vec3 position = glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f))
        : position(position),
          target(target),
          currentOrientation(DeviceOrientation::Portrait0),
          aspectRatio(1.0f),
          fov(45.0f),
          nearPlane(0.1f),
          farPlane(10.0f),
          worldUp(0.0f, 0.0f, 1.0f) {
        updateVectors();
        updateProjectionMatrix();
    }

    void setOrientation(DeviceOrientation orientation) {
        if (currentOrientation != orientation) {
            currentOrientation = orientation;
            updateProjectionMatrix();
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
        // Calculate rotated up vector based on device orientation
        // This "rolls" the camera around its view direction
        glm::vec3 viewDir = glm::normalize(target - position);
        glm::vec3 rotatedUp = worldUp;

        float angle = 0.0f;
        switch (currentOrientation) {
            case DeviceOrientation::Landscape90:
                angle = glm::radians(-90.0f);
                break;
            case DeviceOrientation::Portrait180:
                angle = glm::radians(180.0f);
                break;
            case DeviceOrientation::Landscape270:
                angle = glm::radians(270.0f);
                break;
            default:
                angle = 0.0f;
                break;
        }

        if (angle != 0.0f) {
            // Rotate up vector around the view direction
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, viewDir);
            rotatedUp = glm::vec3(rotation * glm::vec4(worldUp, 0.0f));
        }

        return glm::lookAt(position, target, rotatedUp);
    }

    glm::mat4 getProjectionMatrix() const {
        return projectionMatrix;
    }

    glm::vec3 getRight() const {
        return right;
    }
    glm::vec3 getUp() const {
        return up;
    }
    glm::vec3 getPosition() const {
        return position;
    }
    glm::vec3 getTarget() const {
        return target;
    }

   private:
    void updateVectors() {
        // worldUp is always constant (0, 0, 1) - Z-up
        // Calculate camera vectors for touch input
        glm::vec3 viewDir = glm::normalize(target - position);
        right = glm::normalize(glm::cross(viewDir, worldUp));
        up = glm::normalize(glm::cross(right, viewDir));
    }

    void updateProjectionMatrix() {
        projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
        projectionMatrix[1][1] *= -1;  // Flip Y for Vulkan
    }

    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 worldUp;
    glm::vec3 right;
    glm::vec3 up;
    DeviceOrientation currentOrientation;

    float aspectRatio;
    float fov;
    float nearPlane;
    float farPlane;
    glm::mat4 projectionMatrix;
};
