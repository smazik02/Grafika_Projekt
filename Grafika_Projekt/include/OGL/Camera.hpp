#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Action {
    NONE = 0,
    FORWARD = 1 << 0,
    BACKWARD = 1 << 1,
    LEFT = 1 << 2,
    RIGHT = 1 << 3,
    JUMP = 1 << 4,
    DOWN = 1 << 5,
    CROUCH = 1 << 6
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float POV = 45.0f;


class Camera {
public:
    glm::vec3 positionVector;
    glm::vec3 frontVector;
    glm::vec3 upVector;
    glm::vec3 rightVector;
    glm::vec3 worldUpVector;

    float yaw;
    float pitch;

    GLfloat movementSpeed;
    GLfloat mouseSensitivity;
    GLfloat pov;

    bool fpsMode;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

    glm::mat4 getViewMatrix() const;

    void setFpsCam(bool fps);

    void processKeyboard(int direction, float deltaTime, bool shiftPressed, bool spacePressed, bool ctrlPressed);

    void processMouse(float xoffset, float yoffset);

private:
    void updateVectors();
};
