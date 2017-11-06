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

using namespace std;

class Graphics {
public:
    Graphics();
    ~Graphics();
    bool Initialize(int width, int height);
    void Update(float dt);
    void Render();
    void moveCylinder(float x, float z);
    float dx, dz;
    usi lighting;
    float full;
    float brightness;
    void increaseAL(float f);
    void increaseSL(float f);
    bool cyanLight;
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

    //These are the shader uniform location variables
    GLint projMat  , viewMat  , modelMat  , normalMat  , tex  , camPos  , ka  , kd  , ks  , shininess  , spotlightPos  , spotlightDir  , spotlightFull  , spotlightFade  , spotlightBrightness  ;
    GLint projMatFL, viewMatFL, modelMatFL, normalMatFL, texFL, camPosFL, kaFL, kdFL, ksFL, shininessFL, spotlightPosFL, spotlightDirFL, spotlightFullFL, spotlightFadeFL, spotlightBrightnessFL;
    GLint projMatVL, viewMatVL, modelMatVL, normalMatVL, texVL, camPosVL, kaVL, kdVL, ksVL, shininessVL, spotlightPosVL, spotlightDirVL, spotlightFullVL, spotlightFadeVL, spotlightBrightnessVL;
    GLint GetUniformLocation(std::string name, Shader * shader) const;
    
    btBroadphaseInterface * broadphase;
    btDefaultCollisionConfiguration * collisionConfiguration;
    btCollisionDispatcher * dispatcher;
    btSequentialImpulseConstraintSolver * solver;
    btDiscreteDynamicsWorld * dynamicsWorld;
};

    
GLuint loadTexture(std::string filename);
void loadMesh(std::string filename, std::vector<VertexData> & vertices, std::vector<unsigned int> & indices, btTriangleMesh * trimesh = nullptr);

#endif /* GRAPHICS_H */
