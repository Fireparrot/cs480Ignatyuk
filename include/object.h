#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <string>
#include <utility>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/color4.h>

#include "graphics_headers.h"

class Graphics;

class Object {
    friend class Graphics;
private:
    btDiscreteDynamicsWorld * dynamicsWorld;
    glm::mat4 model;
    glm::vec3 size;
    const std::vector<VertexData> * vertices;
    const std::vector<unsigned int> * indices;
    glm::vec3 ka, kd, ks;
    float shininess;
    GLuint VB;
    GLuint IB;
    GLuint tex;
    btRigidBody * rigidBody;
public:
    Object(const std::vector<VertexData> * vertices_, const std::vector<unsigned int> * indices_, GLuint tex_, glm::vec3 size_, btDiscreteDynamicsWorld * dynamicsWorld_, const btRigidBody::btRigidBodyConstructionInfo & CI, bool isKinematic = false);
    Object(const std::vector<VertexData> * vertices_, const std::vector<unsigned int> * indices_, GLuint tex_, glm::vec3 pos, glm::quat rot, glm::vec3 size_);
    ~Object();
public:
    void update(float dt);
    void render();

    glm::mat4 & getModel();
    glm::vec3 & getKa();
    glm::vec3 & getKd();
    glm::vec3 & getKs();
    float & getShininess();
    GLuint getTexture() const;
    btRigidBody * getRigidBody();
    
    std::pair<float, glm::vec3> intersect(glm::vec3 start, glm::vec3 ray) const;
};

#endif /* OBJECT_H */
