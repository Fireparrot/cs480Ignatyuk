#include "object.h"

#include <fstream>

Object::Object(const std::string & filename) {  
    /*
    # Blender File for a Cube
    o Cube
    v 1.000000 -1.000000 -1.000000
    v 1.000000 -1.000000 1.000000
    v -1.000000 -1.000000 1.000000
    v -1.000000 -1.000000 -1.000000
    v 1.000000 1.000000 -0.999999
    v 0.999999 1.000000 1.000001
    v -1.000000 1.000000 1.000000
    v -1.000000 1.000000 -1.000000
    s off
    f 2 3 4
    f 8 7 6
    f 1 5 6
    f 2 6 7
    f 7 8 4
    f 1 4 8
    f 1 2 4
    f 5 8 6
    f 2 1 6
    f 3 2 7
    f 3 7 4
    f 5 1 8
    */
    /*
    Vertices = {
        {{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}},
        {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}},
        {{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
        {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
        {{-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}}
    };

    Indices = {
        2, 3, 4,
        8, 7, 6,
        1, 5, 6,
        2, 6, 7,
        7, 8, 4,
        1, 4, 8,
        1, 2, 4,
        5, 8, 6,
        2, 1, 6,
        3, 2, 7,
        3, 7, 4,
        5, 1, 8
    };
    
    // The index works at a 0th index
    for(unsigned int i = 0; i < Indices.size(); i++) {
        Indices[i] = Indices[i] - 1;
    }
    */
    
    std::ifstream file;
    file.open("objects/" + filename);
    if(!file.is_open()) {
        std::cerr << "Could not open file " << filename << " to read as an .obj file!";
    }
    while(!file.eof()) {
        char c;
        file >> c;
        if(c == 'v') {
            float x, y, z;
            file >> x >> y >> z;
            Vertices.push_back({{x, y, z}, {x, y, z}});
        } else if(c == 'f') {
            unsigned int i0, i1, i2;
            file >> i0 >> i1 >> i2;
            Indices.push_back(i0-1);
            Indices.push_back(i1-1);
            Indices.push_back(i2-1);
        } else {while(c != '\n') {file >> c;}}
    }
    
    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
}

Object::~Object() {
    Vertices.clear();
    Indices.clear();
}

void Object::Update(unsigned int dt) {
    /*
    angle1 += dt * rotation * M_PI/1000;
    angle2 += dt * M_PI/1000;
    model = glm::rotate(glm::mat4(1.0f), (angle1/10), glm::vec3(0.0, 1.0, 0.0));
    model *= glm::translate(glm::mat4(1.0f), glm::vec3(10.0, 0.0, 0.0));
    model *= glm::rotate(glm::mat4(1.0f), (angle2), glm::vec3(0.0, 1.0, 0.0));
    */
}

glm::mat4 Object::GetModel() {
    return model;
}
void Object::SetModel(const glm::mat4 & m) {
    model = m;
}

void Object::Render() {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    
    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

