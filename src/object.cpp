#include "object.h"
#include <IL/il.h>

#include <fstream>

//Constructs the object, loading the mesh model using assimp and the texture using devIL
Object::Object(const std::vector<VertexData> * vertices_, const std::vector<unsigned int> * indices_, GLuint tex_, glm::vec3 size_, btDiscreteDynamicsWorld * dynamicsWorld_, const btRigidBody::btRigidBodyConstructionInfo & CI, bool isKinematic):
    vertices(vertices_),
    indices(indices_),
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
    //Later hook up Bullet's quaternion rotations here, if needed
    model = glm::translate(glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ())) *
            glm::rotate(0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) *                                                           
            glm::scale(size);
}

glm::mat4 Object::GetModel() const {
    return model;
}
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

