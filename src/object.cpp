#include "object.h"

#include <fstream>

Object::Object(const std::string & filename) {
    Assimp::Importer importer;
    scene = importer.ReadFile("objects/" + filename, aiProcess_Triangulate);
    
    for(unsigned int i = 0; i < scene->mMeshes[0]->mNumVertices; ++i) {
        const aiVector3D & vert = scene->mMeshes[0]->mVertices[i];
        Vertices.push_back(Vertex{{vert.x, vert.y, vert.z}, {vert.x, vert.y, vert.z}});
    }
    for(unsigned int i = 0; i < scene->mMeshes[0]->mNumFaces; ++i) {
        const aiFace & face = scene->mMeshes[0]->mFaces[i];
        Indices.push_back(face.mIndices[0]);
        Indices.push_back(face.mIndices[1]);
        Indices.push_back(face.mIndices[2]);
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
    delete scene;
}

void Object::Update(unsigned int dt) {}

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

