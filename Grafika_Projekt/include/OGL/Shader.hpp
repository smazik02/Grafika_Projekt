#pragma once

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    // Program ID
    GLuint ID;

    // Constructor reads and builds the shader
    Shader(const char *vertexPath, const char *fragmentPath);

    ~Shader();

    // Use/activate the shader
    void use() const;

    // Utilities
    void setBool(const char *name, GLboolean value) const;

    void setInt(const char* name, GLint value) const;

    void setFloat(const char* name, GLfloat value) const;

    void setVec2(const char* name, const glm::vec2 &value) const;

    void setVec2(const char* name, GLfloat x, GLfloat y) const;

    void setVec3(const char* name, const glm::vec3 &value) const;

    void setVec3(const char* name, GLfloat x, GLfloat y, GLfloat z) const;

    void setVec4(const char* name, const glm::vec4 &value) const;

    void setVec4(const char* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const;

    void setMat2(const char* name, const glm::mat2 &mat) const;

    void setMat3(const char* name, const glm::mat3 &mat) const;

    void setMat4(const char* name, const glm::mat4 &mat) const;

    GLint getUniform(const char* name) const;

private:
    GLuint loadShader(GLenum shaderType, const char* filePath);

    char* loadShaderFile(const char* filePath);
};
