#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <map>
#include <string>

#include "graphics_headers.h"

//Nothing is changed in the shader class
class Shader {
public:
    Shader();
    ~Shader();
    bool Initialize();
    void Enable();
    bool AddShader(std::string filename, GLenum ShaderType);
    bool Finalize();
    GLint uniform(std::string pUniformName);

private:
    GLuint m_shaderProg;    
    std::vector<GLuint> m_shaderObjList;
    std::map<std::string, GLint> uniforms;
};

#endif  /* SHADER_H */
