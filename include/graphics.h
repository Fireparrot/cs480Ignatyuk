#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
using namespace std;

#include "graphics_headers.h"
#include "cam.h"
#include "shader.h"
#include "object.h"

class Graphics {
public:
    Graphics();
    ~Graphics();
    bool Initialize(int width, int height);
    void Update(unsigned int dt);
    void Render();
    
    float rotation, orbit;
private:
    std::string ErrorString(GLenum error);

    Cam * m_camera;
    Shader * m_shader;

    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_modelMatrix;

    Object * m_objects[3];
    float a00, a01, a10, a11;
};

#endif /* GRAPHICS_H */
