#include "object.h"
#include <IL/il.h>
#include <glm/gtx/quaternion.hpp>

#include <fstream>

//Constructs the object, loading the mesh model using assimp and the texture using devIL
Object::Object(const std::vector<VertexData> * vertices_, const std::vector<unsigned int> * indices_, GLuint tex_, glm::vec3 size_, btDiscreteDynamicsWorld * dynamicsWorld_, const btRigidBody::btRigidBodyConstructionInfo & CI, bool isKinematic):
    vertices(vertices_),
    indices(indices_),
    ka{0.2f, 0.2f, 0.2f},
    kd{1.0f, 1.0f, 1.0f},
    ks{0.2f, 0.2f, 0.2f},
    shininess(5.f),
    tex(tex_),
    size(size_),
    dynamicsWorld(dynamicsWorld_)
{
    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vertices->size(), &(*vertices)[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices->size(), &(*indices)[0], GL_STATIC_DRAW);
    
    rigidBody = new btRigidBody(CI);
    if(isKinematic) {
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        rigidBody->setActivationState(DISABLE_DEACTIVATION);
    }
    dynamicsWorld->addRigidBody(rigidBody);
}

Object::~Object() {
    dynamicsWorld->removeRigidBody(rigidBody);
    delete rigidBody->getMotionState();
    delete rigidBody;
}

void Object::Update(float dt) {
    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
    const btVector3 & pos = trans.getOrigin();
    const btQuaternion & rot = trans.getRotation();
    model = glm::translate(glm::vec3{pos.getX(), pos.getY(), pos.getZ()}) *
            glm::toMat4(glm::quat{rot.getW(), rot.getX(), rot.getY(), rot.getZ()}) *                                                      
            glm::scale(size);
}

glm::mat4 Object::GetModel() const {return model;}
glm::vec3 & Object::GetKa() {return ka;}
glm::vec3 & Object::GetKd() {return kd;}
glm::vec3 & Object::GetKs() {return ks;}
float & Object::GetShininess() {return shininess;}
GLuint Object::GetTexture() const {return tex;}
btRigidBody * Object::GetRigidBody() {return rigidBody;}

void Object::Render() {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, tex));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, normal));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    
    glDrawElements(GL_TRIANGLES, indices->size(), GL_UNSIGNED_INT, 0);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

