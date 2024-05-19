#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "stb_image.h"

void initOpenGLProgram(GLFWwindow* window);

void freeOpenGLProgram(GLFWwindow* window);

void drawScene(GLFWwindow* window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void mouseCallback(GLFWwindow* window, double mouseXIn, double mouseYIn);

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastMouseX = static_cast<float>(SCR_WIDTH) / 2.0f;
float lastMouseY = static_cast<float>(SCR_HEIGHT) / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

bool flashlight = true;

Shader* myShader;
Model* myModel;

int main() {
    GLFWwindow* window;

    if (!glfwInit()) {
        std::cerr << "Nie mo�na zainicjowa� GLFW\n";
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
    if (!window) {
        std::cerr << "Nie mo�na utworzy� okna\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Nie mo�na zainicjowa� GLEW\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    initOpenGLProgram(window);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = static_cast<GLfloat>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        drawScene(window);

        // check and call events, swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void initOpenGLProgram(GLFWwindow* window) {
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mouseCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    stbi_set_flip_vertically_on_load(true);

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    myShader = new Shader("shaders/myShader.vert", "shaders/myShader.frag");
    // ShaderProgram* myShader = new ShaderProgram("shaders/myShader.vert", NULL, "shaders/myShader.frag");

    myModel = new Model("resources/objects/backpack/backpack.obj");

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void freeOpenGLProgram(GLFWwindow* window) {
    delete myShader;
    delete myModel;
}

void drawScene(GLFWwindow* window) {
    // rendering commands here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myShader->use();
    myShader->setFloat("material.shininess", 64.0f);
    myShader->setVec3("viewPos", camera.positionVector);
    myShader->setBool("flashlight", flashlight);

    myShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    myShader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    myShader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    myShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    //        myShader->setInt("pointLightsCount", 1);
    //        myShader->setVec3("pointLights[0].position", glm::vec3(0.7f, 0.2f, 2.0f));
    //        myShader->setFloat("pointLights[0].constant", 1.0f);
    //        myShader->setFloat("pointLights[0].linear", 0.045f);
    //        myShader->setFloat("pointLights[0].quadratic", 0.0075f);
    //        myShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    //        myShader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    //        myShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);

    myShader->setInt("pointLightsCount", 0);

    myShader->setVec3("spotLight.position", camera.positionVector);
    myShader->setVec3("spotLight.direction", camera.frontVector);
    myShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(15.0f)));
    myShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
    myShader->setFloat("spotLight.constant", 1.0f);
    myShader->setFloat("spotLight.linear", 0.045f);
    myShader->setFloat("spotLight.quadratic", 0.0075f);
    myShader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    myShader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    myShader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);

    glm::mat4 projection = glm::perspective(glm::radians(camera.pov), (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT,
        0.1f, 100.0f);
    myShader->setMat4("projection", projection);
    myShader->setMat4("view", camera.getViewMatrix());

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    myShader->setMat4("model", model);
    // myModel.Draw(myShader);
    myModel->Draw(myShader);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, deltaTime);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
    }
    if (action == GLFW_RELEASE) {}
}

void mouseCallback(GLFWwindow* window, double mouseXIn, double mouseYIn) {
    float mouseX = static_cast<float>(mouseXIn);
    float mouseY = static_cast<float>(mouseYIn);

    if (firstMouse) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouse = false;
    }

    float xOffset = mouseX - lastMouseX;
    float yOffset = lastMouseY - mouseY;

    lastMouseX = mouseX;
    lastMouseY = mouseY;

    camera.processMouse(xOffset, yOffset);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) flashlight = !flashlight;
    }
}