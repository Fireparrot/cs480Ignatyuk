#include "graphics.h"
#include <fstream>

using usi = unsigned short int;

Graphics::Graphics() {}

Graphics::~Graphics() {
    for(Object * object : objects) {delete object;}
    for(std::vector<VertexData> * vertices : vertexVectors) {delete vertices;}
    for(std::vector<unsigned int> * indices : indexVectors) {delete indices;}
    for(btCollisionShape * shape : collisionShapes) {delete shape;}
    for(btRigidBody * rigidBody : rigidBodies) {delete rigidBody;}
    delete dynamicsWorld;
    delete solver;
    delete collisionConfiguration;
    delete dispatcher;
    delete broadphase;
}

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
        printf("Program failed to Finalize\n");
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
    
    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    //Culls (i.e. doesn't render) back faces from triangles
    glEnable(GL_CULL_FACE);
    
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0, -10, 0));
    
    for(usi i = 0; i < 3; ++i) {
        vertexVectors.push_back(new std::vector<VertexData>());
        indexVectors.push_back(new std::vector<unsigned int>());
    }
    loadMesh("box.obj",         *(vertexVectors[0]), *(indexVectors[0]));
    loadMesh("cylinder.obj",    *(vertexVectors[1]), *(indexVectors[1]));
    loadMesh("sphere.obj",      *(vertexVectors[2]), *(indexVectors[2]));
    for(usi i = 0; i < 1; ++i) {
        texes.push_back(GLuint(0));
    }
    //lol
    texes[0] = loadTexture("Earth.png");
    
    collisionShapes.push_back(new btBoxShape({1.0f, 1.0f, 1.0f}));
    collisionShapes.push_back(new btCylinderShape({4.0f, 4.0f, 4.0f}));
    collisionShapes.push_back(new btSphereShape(1.0f));
    
    collisionShapes.push_back(new btBoxShape({20.0f, 1.0f, 20.0f}));
    collisionShapes.push_back(new btBoxShape({20.0f, 1.0f,  1.0f}));
    collisionShapes.push_back(new btBoxShape({ 1.0f, 1.0f, 20.0f}));
    
    btRigidBody::btRigidBodyConstructionInfo CI(
        0,
        new btDefaultMotionState(btTransform(
            btQuaternion(0, 0, 0, 1),
            btVector3(-5, 1, 0)
        )),
        collisionShapes[0],
        btVector3(0.0f, 0.0f, 0.0f)
    );
    objects.push_back(new Object(
        vertexVectors[0],
        indexVectors[0],
        texes[0],
        {1.0f, 1.0f, 1.0f},
        dynamicsWorld,
        CI,
        true
    ));
    
    CI = btRigidBody::btRigidBodyConstructionInfo(
        0,
        new btDefaultMotionState(btTransform(
            btQuaternion(0, 0, 0, 1),
            btVector3(0, 2, 0)
        )),
        collisionShapes[1],
        btVector3(0.0f, 0.0f, 0.0f)
    );
    CI.m_restitution = 4.0f;
    objects.push_back(new Object(
        vertexVectors[1],
        indexVectors[1],
        texes[0],
        {4.0f, 1.0f, 4.0f},
        dynamicsWorld,
        CI
    ));
    
    btVector3 inertia(0.0f, 0.0f, 0.0f);
    collisionShapes[2]->calculateLocalInertia(5.0f, inertia);
    CI = btRigidBody::btRigidBodyConstructionInfo(
        5,
        new btDefaultMotionState(btTransform(
            btQuaternion(0, 0, 0, 1),
            btVector3(-5, 1, 5)
        )),
        collisionShapes[2],
        inertia
    );
    CI.m_restitution = 1.0f;
    objects.push_back(new Object(
        vertexVectors[2],
        indexVectors[2],
        texes[0],
        {1.0f, 1.0f, 1.0f},
        dynamicsWorld,
        CI
    ));
    
    
    
    CI = btRigidBody::btRigidBodyConstructionInfo(
        0,
        new btDefaultMotionState(btTransform(
            btQuaternion(0, 0, 0, 1),
            btVector3(0, 0, 0)
        )),
        collisionShapes[3],
        btVector3(0.0f, 0.0f, 0.0f)
    );
    CI.m_restitution = 0.5f;
    objects.push_back(new Object(
        vertexVectors[0],
        indexVectors[0],
        texes[0],
        {20.0f, 1.0f, 20.0f},
        dynamicsWorld,
        CI
    ));
    
    CI = btRigidBody::btRigidBodyConstructionInfo(
        0,
        new btDefaultMotionState(btTransform(
            btQuaternion(0, 0, 0, 1),
            btVector3(0, 1, 20)
        )),
        collisionShapes[4],
        btVector3(0.0f, 0.0f, 0.0f)
    );
    CI.m_restitution = 0.5f;
    objects.push_back(new Object(
        vertexVectors[0],
        indexVectors[0],
        texes[0],
        {20.0f, 1.0f, 1.0f},
        dynamicsWorld,
        CI
    ));
    
    CI = btRigidBody::btRigidBodyConstructionInfo(
        0,
        new btDefaultMotionState(btTransform(
            btQuaternion(0, 0, 0, 1),
            btVector3(0, 1, -20)
        )),
        collisionShapes[4],
        btVector3(0.0f, 0.0f, 0.0f)
    );
    CI.m_restitution = 0.5f;
    objects.push_back(new Object(
        vertexVectors[0],
        indexVectors[0],
        texes[0],
        {20.0f, 1.0f, 1.0f},
        dynamicsWorld,
        CI
    ));
    
    CI = btRigidBody::btRigidBodyConstructionInfo(
        0,
        new btDefaultMotionState(btTransform(
            btQuaternion(0, 0, 0, 1),
            btVector3(20, 1, 0)
        )),
        collisionShapes[5],
        btVector3(0.0f, 0.0f, 0.0f)
    );
    CI.m_restitution = 0.5f;
    objects.push_back(new Object(
        vertexVectors[0],
        indexVectors[0],
        texes[0],
        {1.0f, 1.0f, 20.0f},
        dynamicsWorld,
        CI
    ));
    
    CI = btRigidBody::btRigidBodyConstructionInfo(
        0,
        new btDefaultMotionState(btTransform(
            btQuaternion(0, 0, 0, 1),
            btVector3(-20, 1, 0)
        )),
        collisionShapes[5],
        btVector3(0.0f, 0.0f, 0.0f)
    );
    CI.m_restitution = 0.5f;
    objects.push_back(new Object(
        vertexVectors[0],
        indexVectors[0],
        texes[0],
        {1.0f, 1.0f, 20.0f},
        dynamicsWorld,
        CI
    ));
    
    
    
    return true;
}

void Graphics::moveCylinder(float x, float z) {
    btTransform trans;
    objects[0]->GetRigidBody()->getMotionState()->getWorldTransform(trans);
    trans.setOrigin(btVector3(trans.getOrigin().getX() + x, trans.getOrigin().getY(), trans.getOrigin().getZ() + z));
    objects[0]->GetRigidBody()->getMotionState()->setWorldTransform(trans);
    //std::cout << "Position: " << trans.getOrigin().getX() << ", " << trans.getOrigin().getY() << ", " << trans.getOrigin().getZ() << std::endl;
}

void Graphics::Update(float dt) {
    dynamicsWorld->stepSimulation(dt, 10);
    for(Object * object : objects) {
        object->Update(dt);
    }
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

GLint Graphics::GetUniformLocation(std::string name) const {
    GLint loc = m_shader->GetUniformLocation(name.c_str());
    if(loc == INVALID_UNIFORM_LOCATION) {
        std::cout << name << " not found" << std::endl;
    }
    return loc;
}

//This is a helper function that uses devIL to open an image, then loads it into openGL and passes back the openGL reference to the texture
GLuint loadTexture(std::string filename) {
    ILuint image;
    ilGenImages(1, &image);
    ilBindImage(image);
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    ilLoadImage(("objects/textures/" + filename).c_str());
    
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, (ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL) == 3 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE, ilGetData());
    glBindTexture(GL_TEXTURE_2D, 0);
    
    ilDeleteImages(1, &image);
    
    return tex;
}
void loadMesh(std::string filename, std::vector<VertexData> & vertices, std::vector<unsigned int> & indices) {
    Assimp::Importer importer;
    const aiScene * scene = importer.ReadFile("objects/models/" + filename, aiProcess_Triangulate);
    
    aiVector3D zero3D(0.f, 0.f, 0.f);
    
    for(unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        const aiMesh * mesh = scene->mMeshes[m];
        for(unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            const aiVector3D * pos = &(mesh->mVertices[i]);
            const aiVector3D * st;
            if(mesh->HasTextureCoords(0)) {
                st = &(mesh->mTextureCoords[0][i]);
            } else {
                st = &zero3D;
            }
            const aiVector3D * norm = &(mesh->mNormals[i]);
            if(&mesh->mNormals[0] == NULL || norm == NULL) {norm = &zero3D;}
            vertices.push_back(VertexData{{pos->x, pos->y, pos->z}, {st->x, st->y}, {norm->x, norm->y, norm->z}});
        }
        for(unsigned int i = 0; i < scene->mMeshes[m]->mNumFaces; ++i) {
            const aiFace & face = scene->mMeshes[m]->mFaces[i];
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }
    }
}

