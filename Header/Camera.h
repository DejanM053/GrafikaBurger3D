#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    Camera() :
        position(10.0f, 2.0f, 5.0f),
        worldUp(0.0f, 1.0f, 0.0f),
        yaw(-90.0f),
        pitch(0.0f),
        fov(45.0f),
        nearPlane(0.1f),
        farPlane(100.0f)
    {
        updateCameraVectors();
    }

    void updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }

    glm::mat4 getViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 getProjectionMatrix(float aspectRatio) {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }

    // Get orthographic projection matrix for 2D UI overlay
    // This creates a fixed 2D coordinate system from -1 to 1 on both axes
    glm::mat4 getOrthoProjectionMatrix() {
        return glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    }

    // Get identity view matrix for UI (no camera transformation)
    glm::mat4 getUIViewMatrix() {
        return glm::mat4(1.0f);
    }

    void processKeyboard(int key, float deltaTime, bool allowMovement) {
        if (!allowMovement) return;

        float velocity = 2.5f * deltaTime;

        if (key == GLFW_KEY_UP)
            position += front * velocity;
        if (key == GLFW_KEY_DOWN)
            position -= front * velocity;
        if (key == GLFW_KEY_LEFT)
            position -= right * velocity;
        if (key == GLFW_KEY_RIGHT)
            position += right * velocity;
    }

    void processMouseMovement(double xpos, double ypos, bool allowRotation) {
        if (!allowRotation) return;

        static bool firstMouse = true;
        static double lastX = 400, lastY = 300;

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // Constrain pitch
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        updateCameraVectors();
    }
};
