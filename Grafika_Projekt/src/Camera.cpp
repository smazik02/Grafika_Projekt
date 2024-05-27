#include <OGL/Camera.hpp>
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) {
    positionVector = position;
    frontVector = glm::vec3(0.0f, 0.0f, -1.0f);
    worldUpVector = up;

    this->yaw = yaw;
    this->pitch = pitch;

    movementSpeed = SPEED;
    mouseSensitivity = SENSITIVITY;
    pov = POV;

    fpsMode = true;

    updateVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(positionVector, positionVector + frontVector, upVector);
}

void Camera::setFpsCam(bool fps) {
    fpsMode = fps;
    movementSpeed = fpsMode ? SPEED : SPEED * 4.0f;
}

void Camera::processKeyboard(int direction, float deltaTime, bool shiftPressed, bool spacePressed, bool ctrlPressed) {
    float velocity = movementSpeed * deltaTime;

    if (shiftPressed) velocity *= 2.0f;

    // Handle crouching
    static bool isCrouching = false;
    static float crouchVelocity = 0.0f;

    if ((direction & CROUCH) && !spacePressed) {
        isCrouching = true;
        crouchVelocity = 1.0f * deltaTime; // Smooth crouch
        positionVector.y -= crouchVelocity;

        if (positionVector.y <= -0.5f) {
            positionVector.y = -0.5f;
        }
    }
    else if (positionVector.y < 0.0f && isCrouching && !ctrlPressed) {
        crouchVelocity = 1.0f * deltaTime;
        positionVector.y += crouchVelocity;
        if (positionVector.y > 0.0f) {
            positionVector.y = 0.0f;
            isCrouching = false;
        }
    }

    // Handle jumping
    static bool isJumping = false;  // Track if the camera is currently jumping
    static bool isFalling = false;  // Track if the camera is currently falling
    static float jumpVelocity = 0.0f;

    if ((direction & JUMP) && !ctrlPressed && positionVector.y == 0.0f && !isJumping && !isCrouching) {
        isJumping = true;
        jumpVelocity = 0.003f; // Initial jump velocity
    }

    if (isJumping) {
        if (positionVector.y >= 0.5f) {
            isFalling = true;
            jumpVelocity -= 0.0015f; // Gravity effect
        }
        positionVector.y += jumpVelocity;

        if (positionVector.y <= 0.0f) {
            positionVector.y = 0.0f;
            isJumping = false;
            isFalling = false;
        }
    }

    // Handle directional movement
    if (direction & FORWARD) {
        if (fpsMode)
            positionVector += glm::cross(worldUpVector, rightVector) * velocity;
        else
            positionVector += frontVector * velocity;
    }
    if (direction & BACKWARD) {
        if (fpsMode)
            positionVector -= glm::cross(worldUpVector, rightVector) * velocity;
        else
            positionVector -= frontVector * velocity;
    }
    if (direction & LEFT)
        positionVector -= rightVector * velocity;
    if (direction & RIGHT)
        positionVector += rightVector * velocity;

    /*if (!fpsMode) {
        if (direction & UP)
            positionVector += worldUpVector * velocity / 2.0f;
        if (direction & DOWN)
            positionVector -= worldUpVector * velocity / 2.0f;
    }*/
}

void Camera::processMouse(float xOffset, float yOffset) {
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    updateVectors();
}

void Camera::updateVectors() {
    frontVector = glm::normalize(glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))
    ));
    rightVector = glm::normalize(glm::cross(frontVector, worldUpVector));
    upVector = glm::normalize(glm::cross(rightVector, frontVector));
}