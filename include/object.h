#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <string>

#include <assimp/Importer.hpp> //includes the importer, which is used to read our obj file
#include <assimp/scene.h> //includes the aiScene object
#include <assimp/postprocess.h> //includes the postprocessing variables for the importer
#include <assimp/color4.h> //includes the aiColor4 object, which is used to handle the colors from the mesh objects 
#include "graphics_headers.h"

class Object {
private:
    glm::mat4 model;
    const aiScene * scene;
    std::vector<VertexData> vertices;
    std::vector<unsigned int> indices;
    GLuint VB;
    GLuint IB;
    GLuint tex;
    float ka, kd, ks;
public:
    Object(const std::string & modelFilename, const std::string & imageFilename, float ka_, float kd_, float ks_);
    ~Object();
public:
    void Update(float dt);
    void Render();

    glm::mat4 GetModel() const;
    void SetModel(const glm::mat4 & model);
    GLuint GetTexture() const;
    float GetKa() const;
    float GetKd() const;
    float GetKs() const;
};

#endif /* OBJECT_H */
