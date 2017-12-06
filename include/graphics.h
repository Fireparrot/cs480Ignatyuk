#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>

#include "graphics_headers.h"
#include "cam.h"
#include "shader.h"
#include "object.h"
#include <IL/il.h>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

class Graphics {
public:
    Graphics();
    ~Graphics();
    bool Initialize(int width, int height);
    void Update(float dt);
    void Render();
    
    int lighting;
    float dx, dy, dz;
    int walkingType;
    float walkingAnimation;
    float crouchingAnimation;
    int peek;
    float peekingAnimation;
    float camTheta, camPhi;
        
    glm::vec3 selectSphere;

private:
    std::string ErrorString(GLenum error);

    Cam * m_camera;
    Shader * shaderFL, * shaderVL;
    
    std::vector<Object *> objects;
    std::vector<std::vector<VertexData> *> vertexVectors;
    std::vector<std::vector<unsigned int> *> indexVectors;
    std::vector<GLuint> texes;
    std::vector<btCollisionShape *> collisionShapes;
    //The rigid bodies stored are those that are not part of an Object -- otherwise the Object would maintain and destroy them
    std::vector<btRigidBody *> rigidBodies;
    
    btBroadphaseInterface * broadphase;
    btDefaultCollisionConfiguration * collisionConfiguration;
    btCollisionDispatcher * dispatcher;
    btSequentialImpulseConstraintSolver * solver;
    btDiscreteDynamicsWorld * dynamicsWorld;
    
    Object * makeObject(btQuaternion orientation, btVector3 position, glm::vec3 size, float mass, float restitution, float friction, usi shapeIndex, usi meshIndex, usi texIndex, bool isKinematic = false);
    Object * makeObject(glm::vec3 pos, glm::quat rot, glm::vec3 size, usi meshIndex, usi texIndex);
};

    
GLuint loadTexture(std::string filename);
void loadMesh(std::string filename, std::vector<VertexData> & vertices, std::vector<unsigned int> & indices, btTriangleMesh * trimesh = nullptr);

#endif /* GRAPHICS_H */
