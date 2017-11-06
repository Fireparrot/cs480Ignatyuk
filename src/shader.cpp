#include "shader.h"
#include <sstream>
#include <fstream>

Shader::Shader() {
    m_shaderProg = 0;
}

Shader::~Shader() {
    for(std::vector<GLuint>::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); it++) {
        glDeleteShader(*it);
    }
    
    if(m_shaderProg != 0) {
        glDeleteProgram(m_shaderProg);
        m_shaderProg = 0;
    }
}

bool Shader::Initialize() {
    m_shaderProg = glCreateProgram();
    
    if(m_shaderProg == 0)  {
        std::cerr << "Error creating shader program\n";
        return false;
    }
    
    return true;
}

// Use this method to add shaders to the program. When finished - call finalize()
bool Shader::AddShader(std::string filename, GLenum ShaderType) {
    std::string s;
    
    std::ostringstream temp;
    std::ifstream file("shaders/" + filename);
    if(!file.is_open()) {
        std::cerr << "Error opening shader file!" << std::endl;
        return false;
    }
    temp << file.rdbuf();
    s = temp.str();
    file.close();
    
    
    GLuint ShaderObj = glCreateShader(ShaderType);
    
    if(ShaderObj == 0)  {
        std::cerr << "Error creating shader type " << ShaderType << std::endl;
        return false;
    }
    
    // Save the shader object - will be deleted in the destructor
    m_shaderObjList.push_back(ShaderObj);
    
    const GLchar* p[1];
    p[0] = s.c_str();
    GLint Lengths[1] = { (GLint)s.size() };
    
    glShaderSource(ShaderObj, 1, p, Lengths);
    
    glCompileShader(ShaderObj);
    
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    
    if(!success)  {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        std::cerr << "Error compiling: " << InfoLog << std::endl;
        return false;
    }
    
    glAttachShader(m_shaderProg, ShaderObj);
    
    return true;
}


// After all the shaders have been added to the program call this function
// to link and validate the program.
bool Shader::Finalize() {
    GLint success = 0;
    GLchar errorLog[1024] {0};
    
    glLinkProgram(m_shaderProg);
    
    glGetProgramiv(m_shaderProg, GL_LINK_STATUS, &success);
    if(success == 0) {
        glGetProgramInfoLog(m_shaderProg, sizeof(errorLog), NULL, errorLog);
        std::cerr << "Error linking shader program: " << errorLog << std::endl;
        return false;
    }
    
    glValidateProgram(m_shaderProg);
    glGetProgramiv(m_shaderProg, GL_VALIDATE_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(m_shaderProg, sizeof(errorLog), NULL, errorLog);
        std::cerr << "Invalid shader program: " << errorLog << std::endl;
        return false;
    }
    
    // Delete the intermediate shader objects that have been added to the program
    for(std::vector<GLuint>::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); it++) {
        glDeleteShader(*it);
    }
    
    m_shaderObjList.clear();
    
    return true;
}


void Shader::Enable() {
    glUseProgram(m_shaderProg);
}

GLint Shader::GetUniformLocation(const char* pUniformName) {
    GLuint location = glGetUniformLocation(m_shaderProg, pUniformName);

    if(location == INVALID_UNIFORM_LOCATION) {
        std::cerr << "Warning! Unable to get the location of uniform" << pUniformName << std::endl;
    }

    return location;
}
