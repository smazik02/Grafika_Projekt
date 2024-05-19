#include "Shader.hpp"

// Tworzenie programu shadera podaj¹c œcie¿ki do plików vertex i fragment shadera
Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    int success;
    char infoLog[512];

    GLuint vertex = loadShader(GL_VERTEX_SHADER, vertexPath);
    GLuint fragment = loadShader(GL_FRAGMENT_SHADER, fragmentPath);

    // Shader program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << "B³¹d linkowania shadera\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

GLuint Shader::loadShader(GLenum shaderType, const char* filePath) {
    GLuint shader;
    int success;
    char infoLog[512];

    const char* shaderSource = loadShaderFile(filePath);

    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "B³¹d kompilacji shadera\n" << infoLog << std::endl;
    }

    delete []shaderSource;
    return shader;
}

char* Shader::loadShaderFile(const char* filePath) {
    int filesize;
    FILE* plik;
    char* result;

    #pragma warning(suppress : 4996)
    plik = fopen(filePath, "rb");
    if (plik != NULL) {
        fseek(plik, 0, SEEK_END);
        filesize = ftell(plik);
        fseek(plik, 0, SEEK_SET);
        result = new char[filesize + 1];
        #pragma warning(suppress : 6386)
        int readsize = fread(result, 1, filesize, plik);
        result[filesize] = 0;
        fclose(plik);

        return result;
    }

    std::cerr << "B³¹d wczytywania pliku shadera\n";
    return NULL;
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

// W³¹czenie shadera
void Shader::use() {
    glUseProgram(ID);
}

// Ustawianie zmiennych jednorodnych shaderów

void Shader::setBool(const char* name, GLboolean value) const {
    glUniform1i(glGetUniformLocation(ID, name), (int) value);
}

void Shader::setInt(const char* name, GLint value) const {
    glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::setFloat(const char* name, GLfloat value) const {
    glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::setVec2(const char* name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(ID, name), 1, &value[0]);
}

void Shader::setVec2(const char* name, GLfloat x, GLfloat y) const {
    glUniform2f(glGetUniformLocation(ID, name), x, y);
}

void Shader::setVec3(const char* name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ID, name), 1, &value[0]);
}

void Shader::setVec3(const char* name, GLfloat x, GLfloat y, GLfloat z) const {
    glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}

void Shader::setVec4(const char* name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(ID, name), 1, &value[0]);
}

void Shader::setVec4(const char* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const {
    glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
}

void Shader::setMat2(const char* name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const char* name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const char* name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
}

// Numer slotu zmiennej jednorodnej o danej nazwie, je¿eli chcesz ustawiaæ je w sposób z labów
GLint Shader::getUniform(const char* name) const {
    return glGetUniformLocation(ID, name);
}