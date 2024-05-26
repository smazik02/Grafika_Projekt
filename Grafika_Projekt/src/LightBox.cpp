#include <OGL/LightBox.hpp>

LightBox::LightBox(glm::vec3 const& position, float constant, float linear, float quadratic, glm::vec3 const& ambient, glm::vec3 const& diffuse, glm::vec3 const& specular) {
	this->position = position;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(0);

	shader = new Shader("shaders/lightShader.vert", "shaders/lightShader.frag");
}

LightBox::~LightBox() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	delete shader;
}

void LightBox::draw(glm::mat4 const& MVP) {
	shader->use();
	shader->setMat4("MVP", MVP);
	shader->setVec3("lightColor", diffuse);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}