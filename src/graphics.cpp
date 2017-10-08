#include "graphics.h"

Graphics::Graphics():
    rotation(1.f),
    orbit(1.f),
    a00(0),
    a01(0),
    a10(0),
    a11(0)
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
    
    // Create the object
    m_objects[0] = new Object("sphere.obj", "Earth.png");
    m_objects[1] = new Object("sphere.obj", "Earth.png");
    m_objects[2] = new Object("sphere.obj", "Earth.png");
    
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

    // Locate the projection matrix in the shader
    m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
    if(m_projectionMatrix == INVALID_UNIFORM_LOCATION) {
        printf("m_projectionMatrix not found\n");
        return false;
    }
    
    // Locate the view matrix in the shader
    m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
    if(m_viewMatrix == INVALID_UNIFORM_LOCATION) {
        printf("m_viewMatrix not found\n");
        return false;
    }
    
    // Locate the model matrix in the shader
    m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");
    if(m_modelMatrix == INVALID_UNIFORM_LOCATION) {
        printf("m_modelMatrix not found\n");
        return false;
    }
    
    m_tex = m_shader->GetUniformLocation("tex");
    if(m_tex == INVALID_UNIFORM_LOCATION) {
        printf("m_tex not found\n");
        return false;
    }
    
    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    return true;
}

void Graphics::Update(unsigned int dt) {
    //Update the objects
    m_objects[0]->Update(dt);
    m_objects[1]->Update(dt);
    m_objects[2]->Update(dt);
    
    //Set the model matrices of the objects
    float baseAngle = dt * M_PI/1000;
    a00 += baseAngle * rotation;                //Planet's rotation
    a01 += baseAngle * orbit / 10;              //Planet's orbit
    a10 += baseAngle / 10;                      //Moon's rotation
    a11 += baseAngle / 30;                      //Moon's orbit
    
    glm::mat4 m0, m1, m2;
    m0  = glm::translate(glm::mat4(1.0f), glm::vec3( 8.0 * cos(a01), 0.0,  8.0 * sin(a01)));
    m0 *= glm::rotate(glm::mat4(1.0f), a00, glm::vec3(0.0, 1.0, 0.0));
    m0 *= glm::scale(glm::mat4(1.0f), glm::vec3(1.f));
    m1  = glm::translate(glm::mat4(1.0f), glm::vec3( 3.0 * cos(a11), 0.0,  3.0 * sin(a11)));
    m1 *= glm::translate(glm::mat4(1.0f), glm::vec3( 8.0 * cos(a01), 0.0,  8.0 * sin(a01)));
    m1 *= glm::rotate(glm::mat4(1.0f), a10, glm::vec3(0.0, 1.0, 0.0));
    m1 *= glm::scale(glm::mat4(1.0f), glm::vec3(0.4f));
    m2  = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.5f, 0.0f));
    m2 *= glm::scale(glm::mat4(1.0f), glm::vec3(6.0f));
    
    m_objects[0]->SetModel(m0);
    m_objects[1]->SetModel(m1);
    m_objects[2]->SetModel(m2);
}

void Graphics::Render() {
    //Clear the screen
    glClearColor(0.0, 0.0, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Start the correct program
    m_shader->Enable();
    
    //Send in the projection and view to the shader
    glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection())); 
    glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(m_tex, 0);
    
    //Render the objects
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_objects[0]->GetModel()));
    glBindTexture(GL_TEXTURE_2D, m_objects[0]->GetTexture());
    m_objects[0]->Render();
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_objects[1]->GetModel()));
    glBindTexture(GL_TEXTURE_2D, m_objects[1]->GetTexture());
    m_objects[1]->Render();
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_objects[2]->GetModel()));
    glBindTexture(GL_TEXTURE_2D, m_objects[2]->GetTexture());
    m_objects[2]->Render();
    
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

