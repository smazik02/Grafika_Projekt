#include <OGL/Camera.hpp>

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

void Camera::processKeyboard(Camera_Movement direction, float deltaTime) {
    // Przesuwanie po³o¿enia postaci w zale¿noœci od kierunku
    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD)
        if (fpsMode)
            positionVector += glm::cross(worldUpVector, rightVector) * velocity;
        else
            positionVector += frontVector * velocity;
    if (direction == BACKWARD)
        if (fpsMode)
            positionVector -= glm::cross(worldUpVector, rightVector) * velocity;
        else
            positionVector -= frontVector * velocity;
    if (direction == LEFT)
        positionVector -= rightVector * velocity;
    if (direction == RIGHT)
        positionVector += rightVector * velocity;

    if (!fpsMode) {
        if (direction == UP)
            positionVector += worldUpVector * velocity / 2.0f;
        if (direction == DOWN)
            positionVector -= worldUpVector * velocity / 2.0f;
    }

    // Kamera w trybie FPS zostaje zawsze na jednym poziomie i nie "lata", zawsze mo¿na to prze³¹czyæ, ale taki by³ warunek projektu
    if (fpsMode) positionVector.y = 0.0f;
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