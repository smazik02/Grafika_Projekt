#define GLEW_STATIC

#include <OGL/Shader.hpp>
#include <OGL/Camera.hpp>
#include <OGL/Model.hpp>
#include <OGL/SkyBox.hpp>
#include <OGL/LightBox.hpp>
#include <STB/stb_image.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>

void initOpenGLProgram(GLFWwindow* window);

void freeOpenGLProgram(GLFWwindow* window);

void setupLighting();

void drawScene(GLFWwindow* window, glm::mat4 const& projection);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void mouseCallback(GLFWwindow* window, double mouseXIn, double mouseYIn);

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

const GLsizei SCR_WIDTH = 1280;
const GLsizei SCR_HEIGHT = 720;

// Tworzy kamerÍ w danej pozycji
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
// Koordynaty startowe kursora do obs≥ugi myszki
float lastMouseX = static_cast<float>(SCR_WIDTH) / 2.0f;
float lastMouseY = static_cast<float>(SCR_HEIGHT) / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// èrÛd≥a úwiat≥a
std::vector <LightBox*> lightSources;
int nLightSources = 1;

// Latarka, do ew. wywalenia, trzeba pomyúleÊ
bool flashlight = false;

Shader *myShader, *noSpecShader;
Model *roof, *column, *ground, *penelope, *torch;
SkyBox* skyBox;

int main() {
    GLFWwindow* window;

    if (!glfwInit()) {
        std::cerr << "Couldn't initialize GLFW\n";
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Couldn't create a window\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Couldn't initialize GLEW\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    initOpenGLProgram(window);

    glm::mat4 projection = glm::perspective(glm::radians(camera.pov), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
    myShader->use();
    myShader->setMat4("Projection", projection);

    setupLighting();

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        drawScene(window, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

// Operacje inicjujπce
void initOpenGLProgram(GLFWwindow* window) {
    // Callbacki zmiany rozmiaru ekranu, klawiatury i myszki
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    // Przechwytywanie myszki przez ekran
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Obraca tekstury podczas ≥adowania, inaczej sπ do gÛry nogami
    stbi_set_flip_vertically_on_load(true);

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // Wczytywanie shaderÛw
    myShader = new Shader("shaders/myShader.vert", "shaders/myShader.frag");
    // myShader = new Shader("shaders/myShader.vert", "shaders/myShaderNoSpec.frag");

    // Wczytywanie modeli
    // myModel = new Model("resources/objects/backpack/backpack.obj");
    // myModel = new Model("resources/objects/greek-column/column.obj");
    roof = new Model("resources/objects/roof/roof.obj");
    column = new Model("resources/objects/greek-column/column.obj");
    ground = new Model("resources/objects/ground/ground.obj");
    penelope = new Model("resources/objects/penelope/penelope.obj");
    torch = new Model("resources/objects/torch2/torch2.obj");

    skyBox = new SkyBox();

    lightSources = {
//                             position          const. linear  quadr.             ambient                         diffuse                      specular
        new LightBox(glm::vec3(5.0f, 1.0f, 5.0f), 1.0f, 0.045f, 0.0075f, glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f))
    };

    //lightBox = new LightBox();
}

void freeOpenGLProgram(GLFWwindow* window) {
    delete myShader;

    delete roof;
    delete column;
    delete ground;
    delete penelope;
    delete torch;

    delete skyBox;
}

void setupLighting() {
    myShader->use();
    myShader->setFloat("material.shininess", 64.0f);

    // Parametry úwiat≥a kierunkowego
    myShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    myShader->setVec3("dirLight.ambient", 0.15f, 0.15f, 0.15f);
    myShader->setVec3("dirLight.diffuse", 0.7f, 0.7f, 0.7f);
    myShader->setVec3("dirLight.specular", 0.6f, 0.6f, 0.6f);

    // Parametry úwiate≥ punktowych
    myShader->setInt("pointLightsCount", nLightSources);
    for (int i = 0; i < nLightSources; i++) {
        std::string pointLights = "pointLights[" + std::to_string(i);
        myShader->setVec3((pointLights + "].position").c_str(), lightSources[i]->position);
        myShader->setFloat((pointLights + "].constant").c_str(), lightSources[i]->constant);
        myShader->setFloat((pointLights + "].linear").c_str(), lightSources[i]->linear);
        myShader->setFloat((pointLights + "].quadratic").c_str(), lightSources[i]->quadratic);
        myShader->setVec3((pointLights + "].ambient").c_str(), lightSources[i]->ambient);
        myShader->setVec3((pointLights + "].diffuse").c_str(), lightSources[i]->diffuse);
        myShader->setVec3((pointLights + "].specular").c_str(), lightSources[i]->specular);
    }

    // Parametry úwiat≥a stoøkowego - latarki
    myShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(15.0f)));
    myShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
    myShader->setFloat("spotLight.constant", 1.0f);
    myShader->setFloat("spotLight.linear", 0.045f);
    myShader->setFloat("spotLight.quadratic", 0.0075f);
    myShader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    myShader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    myShader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
}

void drawScene(GLFWwindow* window, glm::mat4 const& projection) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myShader->use();

    // Pozycja kamery do obliczeÒ oúwietlenia
    myShader->setVec3("viewPos", camera.positionVector);
    myShader->setBool("flashlight", flashlight);

    // Parametry úwiat≥a stoøkowego - latarki
    myShader->setVec3("spotLight.position", camera.positionVector);
    myShader->setVec3("spotLight.direction", camera.frontVector);

    // Macierz rzutowania perspektywicznego oraz widoku
    glm::mat4 view = camera.getViewMatrix();

    // Macierz modelu
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
    myShader->setMat4("Model", model);
    myShader->setMat4("MVP", projection * camera.getViewMatrix() * model);
    ground->draw(myShader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, 0.0f));
    // model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    myShader->setMat4("Model", model);
    myShader->setMat4("MVP", projection * camera.getViewMatrix() * model);
    // Rysowanie modelu
    roof->draw(myShader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 2.0f));
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
    myShader->setMat4("Model", model);
    myShader->setMat4("MVP", projection * camera.getViewMatrix() * model);
    column->draw(myShader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
    myShader->setMat4("Model", model);
    myShader->setMat4("MVP", projection * camera.getViewMatrix() * model);
    penelope->draw(myShader);

    for (int i = 0; i < nLightSources; i++) {
        model = glm::translate(glm::mat4(1.0f), lightSources[i]->position);
        myShader->setMat4("Model", model);
        myShader->setMat4("MVP", projection * camera.getViewMatrix() * model);
        torch->draw(myShader);

        model = glm::scale(model, glm::vec3(0.05f));
        lightSources[i]->draw(projection * camera.getViewMatrix() * model);
    }

    skyBox->draw(camera.getViewMatrix(), projection);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Dwie osobne metody obs≥ugi klawiatury
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.processKeyboard(DOWN, deltaTime);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
        if (key == GLFW_KEY_X) camera.setFpsCam(!camera.fpsMode);
    }
    if (action == GLFW_RELEASE) {}
}

// Obs≥uga ruchu kamery myszkπ
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

// Obs≥ugiwanie przyciskÛw myszki
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) flashlight = !flashlight;
    }
}