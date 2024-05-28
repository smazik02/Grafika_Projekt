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

// Pozycje oraz rozmiary platform
const glm::vec3 platformPos = glm::vec3(12.5f, 0.0f, 8.0f);
const glm::vec3 platformSize = glm::vec3(27.0f, 1.3f, 18.0f);
const glm::vec3 platform2Pos = glm::vec3(13.0f, 1.0f, 8.0f);
const glm::vec3 platform2Size = glm::vec3(21.0f, 1.0f, 14.0f);


bool isEnteringArea(const glm::vec3& position, const glm::vec3& newPosition, const glm::vec3& platformPos, const glm::vec3& platformSize) {

    bool withinXBounds = newPosition.x > platformPos.x - platformSize.x / 2.0f &&
        newPosition.x < platformPos.x + platformSize.x / 2.0f;
    bool withinZBounds = newPosition.z > platformPos.z - platformSize.z / 2.0f &&
        newPosition.z < platformPos.z + platformSize.z / 2.0f;


    if (withinXBounds && withinZBounds && newPosition.y < platformPos.y + platformSize.y) {
        return true;
    }
    return false;
}
float getHeigth(const glm::vec3& position, const glm::vec3& platformPos, const glm::vec3& platformSize) {
	bool withinXBounds = position.x > platformPos.x - platformSize.x / 2.0f &&
		position.x < platformPos.x + platformSize.x / 2.0f;
	bool withinZBounds = position.z > platformPos.z - platformSize.z / 2.0f &&
		position.z < platformPos.z + platformSize.z / 2.0f;


	if (withinXBounds && withinZBounds) {
		return platformPos.y + platformSize.y;
	}
	return 0.0f;
}

void Camera::processKeyboard(int direction, float deltaTime, bool shiftPressed, bool spacePressed, bool ctrlPressed) {
    float velocity = movementSpeed * deltaTime;
    glm::vec3 newPosition = positionVector;
	float CROUCH_SPEED = 2.0f;
	float INITIAL_JUMP_VELOCITY = 0.03f;
	float GRAVITY = 0.05f;



    float floorHeight = 0.0f;
    floorHeight = getHeigth(positionVector, platform2Pos, platform2Size);
	if (floorHeight == 0.0f) {
        floorHeight = getHeigth(positionVector, platformPos, platformSize);
    }
	std::cout << "Floor height: " << floorHeight << std::endl;

	float MAX_CROUCH_HEIGHT = floorHeight - 0.5f;

    if (shiftPressed) velocity *= 2.0f;


    static bool isCrouching = false;
    static float crouchVelocity = 0.0f;

    if ((direction & CROUCH) && !spacePressed) {
        isCrouching = true;
        crouchVelocity = CROUCH_SPEED * deltaTime;
        newPosition.y -= crouchVelocity;

        if (newPosition.y <= MAX_CROUCH_HEIGHT) {
            newPosition.y = MAX_CROUCH_HEIGHT;
        }
    }
    else if (positionVector.y < floorHeight && isCrouching && !ctrlPressed) {
        crouchVelocity = CROUCH_SPEED * deltaTime;
        newPosition.y += crouchVelocity;
        if (newPosition.y > floorHeight) {
            newPosition.y = floorHeight;
            isCrouching = false;
        }
    }

  
    static bool isJumping = false;
    static bool isFalling = false;
    static float jumpVelocity = 0.0f;

    if ((direction & JUMP) && !ctrlPressed && !isJumping && !isCrouching) {
        if (positionVector.y == 0.0f || floorHeight > 0.0f) {
            isJumping = true;
            jumpVelocity = INITIAL_JUMP_VELOCITY;
        }
    }

    if (isJumping) {
        newPosition.y += jumpVelocity;
        jumpVelocity -= GRAVITY * deltaTime;

        if (jumpVelocity <= 0.0f) {
            isJumping = false;
            isFalling = true;
        }
    }

    if ((isFalling || !isCrouching) && fpsMode) {
        newPosition.y += jumpVelocity;
        jumpVelocity -= GRAVITY * deltaTime;

        float platformHeight = floorHeight;
        if (newPosition.y <= 0.0f || (platformHeight > 0.0f && newPosition.y <= platformHeight)) {
            if (newPosition.y <= 0.0f) {
                newPosition.y = 0.0f;
            }
            else if (platformHeight > 0.0f) {
                newPosition.y = platformHeight;
            }
            isFalling = false;
        }
    }

    glm::vec3 movement(0.0f);
    if (direction & FORWARD) {
        if (fpsMode)
            movement += glm::cross(worldUpVector, rightVector) * velocity;
        else
            movement += frontVector * velocity;
    }
    if (direction & BACKWARD) {
        if (fpsMode)
            movement -= glm::cross(worldUpVector, rightVector) * velocity;
        else
            movement -= frontVector * velocity;
    }
    if (direction & LEFT)
        movement -= rightVector * velocity;
    if (direction & RIGHT)
        movement += rightVector * velocity;

    newPosition += movement;
    
	glm::vec3 endPosition = newPosition;

    if (isCrouching)
        newPosition.y = floorHeight + newPosition.y;

    if (isEnteringArea(positionVector, newPosition, platformPos, platformSize) && fpsMode) {
		endPosition = positionVector;
    }
    if (isEnteringArea(positionVector, newPosition, platform2Pos, platform2Size) && fpsMode) {
		endPosition = positionVector;
    }

    positionVector = endPosition;
	
	if (!fpsMode) {
		if (direction & JUMP)
			positionVector += worldUpVector * velocity;
		if (direction & CROUCH)
			positionVector -= worldUpVector * velocity;
	}
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