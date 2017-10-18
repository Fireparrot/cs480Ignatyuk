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

//This is the data structure used to store the data from the config.txt file, which records data about stellar objects
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
    
    
    
    //These variables are made public so that the Engine class can access and change them (because I'm lazy to implement encapsulation access)
    
    //The multiplier of time -- a multiplier of 1 makes one in-simulation day pass in one second, and a multiplier of 2 makes two in-simulation days pass in one second
    float timeMultiplier;
    
    //This divides distances between planets and the sun by this proportion (and divides the distance between planets and their satellites by the square root of this proportion),
    //so that the ridiculous stellar distances can be seen within the simulation at least to scale with each other
    //The distances scaled are calculated off the surface of the body being orbitted
    float distanceDivisor;
    
    //These are camera variables used to store how the camera looks at the planet at its focus -- from what angle, and from how far away
    //The angles are controlled by the mouse, or the up/down/left/right keys, while the distance is controlled by the scroll wheel, through the zoom variable
    float camTheta, camPhi, camDistance;
    
    //This variable stores which planet the camera focuses on -- a 1 corresponds to the first planet in the solar system, Mercury, and 9 corresponds to the last "planet", Pluto
    unsigned short int planetFocus;
    
    //This variable determines how far away from the planet the camera is, with a minimum boundary. The distances are relative to the planet's size
    float zoom;
private:
    std::string ErrorString(GLenum error);

    Cam * m_camera;
    Shader * m_shader;

    //These are the shader uniform location variables
    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_modelMatrix;
    GLint m_tex;
    GLint m_ka, m_kd, m_ks;
    GLint m_camPos;
    GLint m_reflectOnlyBlue;
    GLint m_sunR, m_shadowP, m_shadowR;

    //These store the data from the config.txt file, as well as information derived from it (and the passage if time, if relevant)
    float sunRadius;
    std::vector<Object *> objects;
    std::vector<StellarData> data;
    std::vector<float> rotations;
    std::vector<float> orbits;
    
    //These are helper functions used to calculate useful information, such as where a planet is in its orbit
    float sizeOf(std::string objectName) const;
    unsigned int findIndex(std::string name) const;
    glm::mat4 orbitPosition(std::string objectName) const;
    GLint GetUniformLocation(std::string name) const;
};

#endif /* GRAPHICS_H */
