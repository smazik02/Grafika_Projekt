#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <string>
#include <vector>

#include "Shader.hpp"
#include "stb_image.h"
#include "Mesh.hpp"

class Model {
public:
    std::vector <Mesh> meshes;
    std::string directory;

    Model(char const *path);

    void Draw(Shader *shader);

private:
    std::vector <Texture> texturesLoaded;

    void loadModel(std::string const &path);

    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector <Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

    static GLuint TextureFromFile(const char *path, const std::string &directory);
};
