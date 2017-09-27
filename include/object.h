#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <string>
#include "graphics_headers.h"

class Object {
private:
    glm::mat4 model;
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    GLuint VB;
    GLuint IB;
public:
    Object(const std::string & filename);
    ~Object();
public:
    void Update(unsigned int dt);
    void Render();

    glm::mat4 GetModel();
    void SetModel(const glm::mat4 & model);

};

#endif /* OBJECT_H */
