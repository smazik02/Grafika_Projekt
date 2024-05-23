#pragma once

#include <GL/glew.h>
#include <iostream>
#include <string>
#include <vector>

#include "Shader.hpp"
#include "stb_image.h"

class SkyBox {
public:
	SkyBox();
	~SkyBox();
	void draw(glm::mat4 const &view, glm::mat4 const&projection);
private:
	GLuint VAO, VBO, tex;
	float *verticies;
	Shader* shader;

	void loadCubeMap(std::vector<std::string> faces);
};

