#pragma once

#include <OGL/Shader.hpp>
#include <OGL/Mesh.hpp>
#include <STB/stb_image.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <string>
#include <vector>

class Model {
public:
    std::vector <Mesh> meshes;
    std::string directory;

    Model(char const *path);

    void draw(Shader *shader);

private:
    std::vector <Texture> texturesLoaded;

    void loadModel(std::string const &path);

    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector <Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

    static GLuint textureFromFile(const std::string &path, const std::string &directory);
};