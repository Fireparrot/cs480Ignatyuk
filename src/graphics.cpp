#include "graphics.h"
#include <fstream>

using usi = unsigned short int;

Graphics::Graphics():
    timeMult(1.f),
    distanceMult(1.f),
    camTheta(0),
    camPhi(0),
    camDistance(10),
    planetFocus(3),
    zoom(6.f)
{}

Graphics::~Graphics() {}

bool Graphics::Initialize(int width, int height) {
    //Used for the linux OS
    #if !defined(__APPLE__) && !defined(MACOSX)
        //cout << glewGetString(GLEW_VERSION) << endl;
        glewExperimental = GL_TRUE;
        
        auto status = glewInit();
        
        //This is here to grab the error that comes from glew init.
        //This error is an GL_INVALID_ENUM that has no effects on the performance
        glGetError();
        
        //Check for error
        if(status != GLEW_OK) {
            std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
            return false;
        }
    #endif
    std::cout << "Initialized GL" << std::endl;
    
    ilInit();
    //iluInit();
    //ilutInit();
    //ilutRenderer(ILUT_OPENGL);
    std::cout << "Initialized devIL" << std::endl;
    
    // For OpenGL 3
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // Init Camera
    m_camera = new Cam();
    if(!m_camera->Initialize(width, height)) {
        printf("Camera Failed to Initialize\n");
        return false;
    }
    
    std::ifstream config;
    config.open("config.txt");
    if(!config.is_open()) {
        std::cerr << "Failed to open config.txt!" << std::endl;
        return false;
    }
    
    while(!config.eof()) {
        std::string word;
        config >> word;
        if(config.eof()) {break;}
        if(word == "time") {config >> timeMult; continue;}
        if(word == "distance") {config >> distanceMult; continue;}
        if(word == "#") {while(config.get() != '\n'); continue;}
        StellarData sd;
        sd.name = word;
        config >> sd.size
               >> sd.rotationTime
               >> sd.rotationTilt
               >> sd.orbitTarget
               >> sd.orbitRadius
               >> sd.orbitTime
               >> sd.orbitTilt;
        data.push_back(sd);
        if(sd.name == "Sun") {sunRadius = sd.size;}
        
        std::string meshFilename, textureFilename;
        float ka, kd, ks;
        config >> meshFilename >> textureFilename >> ka >> kd >> ks;
        objects.push_back(new Object(meshFilename, textureFilename, ka, kd, ks));
        
        rotations.push_back(0.f);
        orbits.push_back(0.f);
    }
    
    // Set up the shaders
    m_shader = new Shader();
    if(!m_shader->Initialize()) {
        printf("Shader Failed to Initialize\n");
        return false;
    }
    
    // Add the vertex shader
    if(!m_shader->AddShader(GL_VERTEX_SHADER)) {
        printf("Vertex Shader failed to Initialize\n");
        return false;
    }
    
    // Add the fragment shader
    if(!m_shader->AddShader(GL_FRAGMENT_SHADER)) {
        printf("Fragment Shader failed to Initialize\n");
        return false;
    }
    
    // Connect the program
    if(!m_shader->Finalize()) {
        printf("Program to Finalize\n");
        return false;
    }

    m_projectionMatrix = GetUniformLocation("projectionMatrix");
    if(m_projectionMatrix == INVALID_UNIFORM_LOCATION) {return false;}
    
    m_viewMatrix = GetUniformLocation("viewMatrix");
    if(m_viewMatrix == INVALID_UNIFORM_LOCATION) {return false;}
    
    m_modelMatrix = GetUniformLocation("modelMatrix");
    if(m_modelMatrix == INVALID_UNIFORM_LOCATION) {return false;}
    
    m_tex = GetUniformLocation("tex");
    if(m_tex == INVALID_UNIFORM_LOCATION) {return false;}
    
    m_ka = GetUniformLocation("ka");
    if(m_ka == INVALID_UNIFORM_LOCATION) {return false;}
    
    m_kd = GetUniformLocation("kd");
    if(m_kd == INVALID_UNIFORM_LOCATION) {return false;}
    
    m_ks = GetUniformLocation("ks");
    if(m_ks == INVALID_UNIFORM_LOCATION) {return false;}
    
    m_camPos = GetUniformLocation("camPos");
    if(m_ks == INVALID_UNIFORM_LOCATION) {return false;}
    
    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    
    
    return true;
}

void Graphics::Update(float dt) {
    dt *= timeMult;
    for(usi i = 0; i < objects.size(); ++i) {
    float rt = data[i].rotationTime;
    float ot = data[i].orbitTime;
        if(abs(rt)    > 0.01f) {rotations[i] += 2*float(M_PI)*dt/rt;}
        if(abs(ot)    > 0.01f) {rotations[i] += 2*float(M_PI)*dt/ot;}           //Orbiting accounts for some of the apparent rotation, which needs to be taken away
        if(abs(ot)    > 0.01f) {orbits[i]    += 2*float(M_PI)*dt/ot;}
    }
    
    for(usi i = 0; i < objects.size(); ++i) {
        glm::mat4 modelMat;
        modelMat  = orbitPosition(data[i].name);
        modelMat *= glm::rotate(glm::mat4(1), data[i].rotationTilt/180.f*float(M_PI), glm::vec3(0, 0, 1));
        if(data[i].rotationTime == data[findIndex(data[i].orbitTarget)].rotationTime) {
            modelMat *= glm::rotate(glm::mat4(1), rotations[findIndex(data[i].orbitTarget)], glm::vec3(0, 1, 0));
        } else {
            modelMat *= glm::rotate(glm::mat4(1), rotations[i], glm::vec3(0, 1, 0));
        }
        modelMat *= glm::scale(glm::mat4(1), glm::vec3(data[i].size));
        objects[i]->SetModel(modelMat);
    }
    
    camDistance = data[planetFocus].size * (2+zoom);
    glm::vec3 camTarget = glm::vec3(objects[planetFocus]->GetModel()*glm::vec4(0, 0, 0, 1));
    glm::vec3 camPosition = glm::vec3(glm::translate(glm::mat4(1), glm::vec3(camDistance*cos(camPhi)*cos(camTheta), camDistance*sin(camPhi), camDistance*cos(camPhi)*sin(camTheta))) * glm::vec4(camTarget, 1));
    //glUniform3fv(m_camPos, 1, glm::value_ptr(camPosition));
    const StellarData & od = data[planetFocus];
    //std::cerr << od.name << "/" << od.size << "/" << od.rotationTime << "/" << od.rotationTilt << "/" << od.orbitTarget << "/" << od.orbitRadius << "/" << od.orbitTime << "/" << od.orbitTilt << std::endl;
    m_camera->SetView(camPosition, camTarget);
}

void Graphics::Render() {
    //Clear the screen
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Start the correct program
    m_shader->Enable();
    
    //Send in the projection and view to the shader
    glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection())); 
    glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(m_tex, 0);
    
    //Render the objects
    for(usi i = 0; i < objects.size(); ++i) {
        glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(objects[i]->GetModel()));
        glUniform1f(m_ka, objects[i]->GetKa());
        glUniform1f(m_kd, objects[i]->GetKd());
        glUniform1f(m_ks, objects[i]->GetKs());
        glBindTexture(GL_TEXTURE_2D, objects[i]->GetTexture());
        objects[i]->Render();
    }
    
    //Get any errors from OpenGL
    auto error = glGetError();
    if(error != GL_NO_ERROR) {
        string val = ErrorString(error);
        std::cout<< "Error within OpenGL! " << error << ", " << val << std::endl;
    }
}

std::string Graphics::ErrorString(GLenum error) {
    if(error == GL_INVALID_ENUM) {
        return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
    } else if(error == GL_INVALID_VALUE) {
        return "GL_INVALID_VALUE: A numeric argument is out of range.";
    } else if(error == GL_INVALID_OPERATION) {
        return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
    } else if(error == GL_INVALID_FRAMEBUFFER_OPERATION) {
        return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
    } else if(error == GL_OUT_OF_MEMORY) {
        return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
    } else {
        return "None";
    }
}

float Graphics::sizeOf(std::string name) const {
    unsigned int i = 0;
    for(const StellarData & sd : data) {if(sd.name == name) {break;} ++i;}
    if(i == data.size()) {return 0.f;}
    return data[i].size;
}
unsigned int Graphics::findIndex(std::string name) const {
    unsigned int i = 0;
    for(const StellarData & sd : data) {if(sd.name == name) {break;} ++i;}
    if(i == data.size()) {return 0;}
    return i;
}

glm::mat4 Graphics::orbitPosition(std::string objectName) const {
    unsigned int i = 0;
    for(const StellarData & sd : data) {if(sd.name == objectName) {break;} ++i;}
    if(i == data.size()) {return glm::mat4(1.0f);}
    const StellarData & sd = data[i];
    if(sd.name == sd.orbitTarget) {
        float r = sd.orbitRadius;
        float a = orbits[i];
        float t = sd.orbitTilt/180.f*float(M_PI);
        return glm::translate(glm::mat4(1.0f), glm::vec3(r*cos(a), -r*sin(a)*sin(t), -r*sin(a)*cos(t)));
    } else {
        float r = sd.orbitRadius;
        float a = orbits[i];
        float t = sd.orbitTilt/180.f*float(M_PI);
        if(r > 0) {
            if(sd.orbitTarget == "Sun") {r -= sunRadius; r /= distanceMult; r += sunRadius;}
            else {r -= sizeOf(sd.orbitTarget); r /= sqrt(distanceMult); r += sizeOf(sd.orbitTarget);}
        }
        return glm::translate(glm::mat4(1.0f), glm::vec3(r*cos(a), -r*sin(a)*sin(t), -r*sin(a)*cos(t)))*orbitPosition(sd.orbitTarget);
    }
}

GLint Graphics::GetUniformLocation(std::string name) const {
    GLint loc = m_shader->GetUniformLocation(name.c_str());
    if(loc == INVALID_UNIFORM_LOCATION) {
        std::cout << name << " not found" << std::endl;
    }
    return loc;
}

