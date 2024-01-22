#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) :
        cameraPosition(cameraPosition),
        cameraTarget(cameraTarget),
        cameraUpDirection(cameraUp),
        yaw(-90.0f),
        pitch(0.0f) {
        // Initialize other members if needed
    }

    glm::vec3 Camera::getPosition() {
        return this->cameraPosition;
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO

        return glm::lookAt(cameraPosition, cameraTarget, this->cameraUpDirection);
    }

    void Camera::setPosition(glm::vec3 pos) {
        this->cameraPosition = pos;
    }

    void Camera::setCameraTarget(glm::vec3 pos) {
        this->cameraTarget = pos;
    }

    glm::vec3 Camera::getCameraTarget() {
        return this->cameraTarget;
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        glm::vec3 directionVector;

        // Calculate the forward (view) vector from the camera's target and position
        glm::vec3 forwardVector = glm::normalize(cameraTarget - cameraPosition);

        // Calculate the right vector as perpendicular to the forward vector and up vector
        glm::vec3 rightVector = glm::normalize(glm::cross(forwardVector, cameraUpDirection));

        switch (direction) {
        case MOVE_FORWARD:
            directionVector = forwardVector;
            break;
        case MOVE_BACKWARD:
            directionVector = -forwardVector;
            break;
        case MOVE_LEFT:
            directionVector = -rightVector;
            break;
        case MOVE_RIGHT:
            directionVector = rightVector;
            break;
        }

        // Update camera position
        cameraPosition += speed * directionVector;
        cameraTarget += speed * directionVector;
    }


    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitchChange, float yawChange) {
        yaw += yawChange;
        pitch += pitchChange;

        // Constrain the pitch
        pitch = glm::clamp(pitch, -89.0f, 89.0f);

        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFrontDirection = glm::normalize(front);

        // Update camera target or front direction
        cameraTarget = cameraPosition + cameraFrontDirection;
    }


}
