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

struct StellarData {
    std::string name;
    float size;
    float rotationTime;
    float rotationTilt;
    std::string orbitTarget;
    float orbitRadius;
    float orbitTime;
    float orbitTilt;
};

class Graphics {
public:
    Graphics();
    ~Graphics();
    bool Initialize(int width, int height);
    void Update(float dt);
    void Render();
    float timeMult;
    float distanceMult;
    float camTheta, camPhi, camDistance;
    unsigned short int planetFocus;
    float zoom;
private:
    std::string ErrorString(GLenum error);

    Cam * m_camera;
    Shader * m_shader;

    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_modelMatrix;
    GLint m_tex;
    GLint m_ka, m_kd, m_ks;
    GLint m_camPos;

    float sunRadius;
    std::vector<Object *> objects;
    std::vector<StellarData> data;
    std::vector<float> rotations;
    std::vector<float> orbits;
    
    float sizeOf(std::string objectName) const;
    unsigned int findIndex(std::string name) const;
    glm::mat4 orbitPosition(std::string objectName) const;
    GLint GetUniformLocation(std::string name) const;
};

#endif /* GRAPHICS_H */
