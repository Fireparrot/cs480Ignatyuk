#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/color4.h>

#include "graphics_headers.h"

class Graphics;

class Object {
private:
    btDiscreteDynamicsWorld * dynamicsWorld;
    glm::mat4 model;
    glm::vec3 size;
    const std::vector<VertexData> * vertices;
    const std::vector<unsigned int> * indices;
    GLuint VB;
    GLuint IB;
    GLuint tex;
    btRigidBody * rigidBody;
public:
    Object(const std::vector<VertexData> * vertices_, const std::vector<unsigned int> * indices_, GLuint tex_, glm::vec3 size_, btDiscreteDynamicsWorld * dynamicsWorld_, const btRigidBody::btRigidBodyConstructionInfo & CI, bool isKinematic = false);
    ~Object();
public:
    void Update(float dt);
    void Render();

    glm::mat4 GetModel() const;
    GLuint GetTexture() const;
    btRigidBody * GetRigidBody();
};

#endif /* OBJECT_H */
