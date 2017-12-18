#include "object.h"
#include <IL/il.h>
#include <glm/gtx/quaternion.hpp>

#include <fstream>
#include <utility>
#include <cmath>

//Constructs the object, loading the mesh model using assimp and the texture using devIL
Object::Object(const std::vector<VertexData> * vertices_, const std::vector<unsigned int> * indices_, GLuint tex_, glm::vec3 size_, btDiscreteDynamicsWorld * dynamicsWorld_, const btRigidBody::btRigidBodyConstructionInfo & CI, bool isKinematic):
    vertices(vertices_),
    indices(indices_),
    ka{0.2f, 0.2f, 0.2f},
    kd{1.0f, 1.0f, 1.0f},
    ks{0.0f, 0.0f, 0.0f},
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

Object::Object(const std::vector<VertexData> * vertices_, const std::vector<unsigned int> * indices_, GLuint tex_, glm::vec3 pos, glm::quat rot, glm::vec3 size_):
    vertices(vertices_),
    indices(indices_),
    ka{0.2f, 0.2f, 0.2f},
    kd{1.0f, 1.0f, 1.0f},
    ks{0.2f, 0.2f, 0.2f},
    shininess(5.f),
    tex(tex_),
    size(size_),
    dynamicsWorld(NULL),
    rigidBody(NULL)
{
    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vertices->size(), &(*vertices)[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices->size(), &(*indices)[0], GL_STATIC_DRAW);
    
    model = glm::translate(pos) *
            glm::toMat4(rot) *                                                      
            glm::scale(size);
}

Object::~Object() {
    if(rigidBody) {
        dynamicsWorld->removeRigidBody(rigidBody);
        delete rigidBody->getMotionState();
        delete rigidBody;
    }
}

void Object::update(float dt) {
    if(rigidBody) {
        btTransform trans;
        rigidBody->getMotionState()->getWorldTransform(trans);
        const btVector3 & pos = trans.getOrigin();
        const btQuaternion & rot = trans.getRotation();
        model = glm::translate(glm::vec3{pos.getX(), pos.getY(), pos.getZ()}) *
                glm::toMat4(glm::quat{rot.getW(), rot.getX(), rot.getY(), rot.getZ()}) *                                                      
                glm::scale(size);
    }
}

glm::mat4 & Object::getModel() {return model;}
glm::vec3 & Object::getKa() {return ka;}
glm::vec3 & Object::getKd() {return kd;}
glm::vec3 & Object::getKs() {return ks;}
float & Object::getShininess() {return shininess;}
GLuint Object::getTexture() const {return tex;}
btRigidBody * Object::getRigidBody() {return rigidBody;}

void Object::render() {
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

std::pair<float, glm::vec3> Object::intersect(glm::vec3 start, glm::vec3 ray) const {
    float lambda = 0;
    glm::vec3 p0 = start;
    for(int i = 0; i < indices->size()/3; ++i) {
        glm::vec3 p1 = vertices->at(indices->at(i*3+0)).position;
        glm::vec3 p2 = vertices->at(indices->at(i*3+1)).position;
        glm::vec3 p3 = vertices->at(indices->at(i*3+2)).position;
        p1 = (glm::vec3)(model*glm::vec4(p1, 1));
        p2 = (glm::vec3)(model*glm::vec4(p2, 1));
        p3 = (glm::vec3)(model*glm::vec4(p3, 1));
        
        glm::vec3 n = glm::normalize(glm::cross(p3-p1, p2-p1));
        glm::vec3 d1 = glm::cross(p3-p2, n);
        glm::vec3 d2 = glm::cross(p1-p3, n);
        glm::vec3 d3 = glm::cross(p2-p1, n);
        if(glm::dot(n, ray) > 0) {n *= -1;}
        
        glm::vec3 testP0;
        float testLambda = (glm::dot(n, p1)-glm::dot(n, start))/glm::dot(n, ray);
        if(!std::isinf(testLambda) && testLambda > 0 && (testLambda < lambda || lambda == 0)) {
            testP0 =  testLambda*ray + start;
            if(glm::dot(d1, testP0-p3) >= 0 && glm::dot(d2, testP0-p1) >= 0 && glm::dot(d3, testP0-p2) >= 0) {p0 = testP0; lambda = testLambda;}
        }
    }
    return std::pair<float, glm::vec3>(lambda, p0);
}

