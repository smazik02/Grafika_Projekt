#pragma once

#include <GL/glew.h>

#include <OGL/Shader.hpp>
#include <STB/stb_image.h>

#include <iostream>
#include <string>
#include <vector>

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

