#include "object.h"
#define ILUT_USE_OPENGL
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <fstream>

GLuint loadTexture(const char * filename) {
    ILuint image;
    ilGenImages(1, &image);
    ilBindImage(image);
    ilLoadImage(filename);
    
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGB, GL_UNSIGNED_BYTE, ilGetData());
    
    ilDeleteImages(1, &image);
    
    return tex;
}

Object::Object(const std::string & filename, const std::string & imageFilename) {
    Assimp::Importer importer;
    scene = importer.ReadFile("objects/models/" + filename, aiProcess_Triangulate);
    
    aiVector3D zero3D(0.f, 0.f, 0.f);
    
    for(unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        const aiMesh * mesh = scene->mMeshes[m];
        for(unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            const aiVector3D * pos = &(mesh->mVertices[i]);
            const aiVector3D * tex;
            if(mesh->HasTextureCoords(0)) {
                tex = &(mesh->mTextureCoords[0][i]);
            } else {
                tex = &zero3D;
            }
            const aiVector3D * norm = &(mesh->mNormals[i]);
            if(&mesh->mNormals[0] == NULL || norm == NULL) {norm = &zero3D;}
            vertices.push_back(VertexData{{pos->x, pos->y, pos->z}, {tex->x, tex->y}, {norm->x, norm->y, norm->z}});
        }
        for(unsigned int i = 0; i < scene->mMeshes[m]->mNumFaces; ++i) {
            const aiFace & face = scene->mMeshes[m]->mFaces[i];
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }
    }
    
    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
    
    tex = loadTexture(("objects/textures/"+imageFilename).c_str());
    //std::cout << "tex" << tex << std::endl;
}

Object::~Object() {
    vertices.clear();
    indices.clear();
    //delete scene;
}

void Object::Update(unsigned int dt) {}

glm::mat4 Object::GetModel() const {
    return model;
}
void Object::SetModel(const glm::mat4 & m) {
    model = m;
}
GLuint Object::GetTexture() const {return tex;}

void Object::Render() {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, tex));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, normal));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

