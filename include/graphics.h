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
private:
    std::string ErrorString(GLenum error);

    Cam * m_camera;
    Shader * m_shader;
    
    std::vector<Object *> objects;
    std::vector<std::vector<VertexData> *> vertexVectors;
    std::vector<std::vector<unsigned int> *> indexVectors;
    std::vector<GLuint> texes;
    std::vector<btCollisionShape *> collisionShapes;
    //The rigid bodies stored are those that are not part of an Object -- otherwise the Object would maintain and destroy them
    std::vector<btRigidBody *> rigidBodies;

    //These are the shader uniform location variables
    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_modelMatrix;
    GLint m_tex;
    GLint GetUniformLocation(std::string name) const;
    
    btBroadphaseInterface * broadphase;
    btDefaultCollisionConfiguration * collisionConfiguration;
    btCollisionDispatcher * dispatcher;
    btSequentialImpulseConstraintSolver * solver;
    btDiscreteDynamicsWorld * dynamicsWorld;
};

    
GLuint loadTexture(std::string filename);
void loadMesh(std::string filename, std::vector<VertexData> & vertices, std::vector<unsigned int> & indices, btTriangleMesh * trimesh = nullptr);

#endif /* GRAPHICS_H */
