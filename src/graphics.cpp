#include "graphics.h"
#include <fstream>

using usi = unsigned short int;

void clip(float & x, float min, float max) {
    if(x < min) {x = min;}
    if(x > max) {x = max;}
}

Graphics::Graphics():
    lighting(3),
    brightness(300.f),
    full(0.8f),
    cyanLight(false)
{}

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
        printf("Camera failed to Initialize\n");
        return false;
    }
    
    // Set up the shaders
    shaderFL = new Shader();
    shaderVL = new Shader();
    if(!shaderFL->Initialize()) {
        printf("Shader (fragment lighting) failed to Initialize\n");
        return false;
    }
    if(!shaderFL->AddShader("shaderFL.vert", GL_VERTEX_SHADER)) {
        printf("Vertex Shader (fragment lighting) failed to Initialize\n");
        return false;
    }
    if(!shaderFL->AddShader("shaderFL.frag", GL_FRAGMENT_SHADER)) {
        printf("Fragment Shader (fragment lighting) failed to Initialize\n");
        return false;
    }
    if(!shaderFL->Finalize()) {
        printf("Program (fragment lighting) failed to Finalize\n");
        return false;
    }
    
    if(!shaderVL->Initialize()) {
        printf("Shader (vertex lighting) failed to Initialize\n");
        return false;
    }
    if(!shaderVL->AddShader("shaderVL.vert", GL_VERTEX_SHADER)) {
        printf("Vertex Shader (vertex lighting) failed to Initialize\n");
        return false;
    }
    if(!shaderVL->AddShader("shaderVL.frag", GL_FRAGMENT_SHADER)) {
        printf("Fragment Shader (vertex lighting) failed to Initialize\n");
        return false;
    }
    if(!shaderVL->Finalize()) {
        printf("Program (vertex lighting) failed to Finalize\n");
        return false;
    }
    

    projMatFL = GetUniformLocation("projectionMatrix", shaderFL);
    if(projMatFL == INVALID_UNIFORM_LOCATION) {return false;}
    viewMatFL = GetUniformLocation("viewMatrix", shaderFL);
    if(viewMatFL == INVALID_UNIFORM_LOCATION) {return false;}
    modelMatFL = GetUniformLocation("modelMatrix", shaderFL);
    if(modelMatFL == INVALID_UNIFORM_LOCATION) {return false;}
    normalMatFL = GetUniformLocation("normalMatrix", shaderFL);
    if(normalMatFL == INVALID_UNIFORM_LOCATION) {return false;}
    
    texFL = GetUniformLocation("tex", shaderFL);
    if(texFL == INVALID_UNIFORM_LOCATION) {return false;}
    camPosFL = GetUniformLocation("camPos", shaderFL);
    if(camPosFL == INVALID_UNIFORM_LOCATION) {return false;}
    kaFL = GetUniformLocation("ka", shaderFL);
    if(kaFL == INVALID_UNIFORM_LOCATION) {return false;}
    kdFL = GetUniformLocation("kd", shaderFL);
    if(kdFL == INVALID_UNIFORM_LOCATION) {return false;}
    ksFL = GetUniformLocation("ks", shaderFL);
    if(ksFL == INVALID_UNIFORM_LOCATION) {return false;}
    shininessFL = GetUniformLocation("shininess", shaderFL);
    if(shininessFL == INVALID_UNIFORM_LOCATION) {return false;}
    
    spotlightPosFL = GetUniformLocation("spotlightPos", shaderFL);
    if(spotlightPosFL == INVALID_UNIFORM_LOCATION) {return false;}
    spotlightDirFL = GetUniformLocation("spotlightDir", shaderFL);
    if(spotlightDirFL == INVALID_UNIFORM_LOCATION) {return false;}
    spotlightFullFL = GetUniformLocation("spotlightFull", shaderFL);
    if(spotlightFullFL == INVALID_UNIFORM_LOCATION) {return false;}
    spotlightFadeFL = GetUniformLocation("spotlightFade", shaderFL);
    if(spotlightFadeFL == INVALID_UNIFORM_LOCATION) {return false;}
    spotlightBrightnessFL = GetUniformLocation("spotlightBrightness", shaderFL);
    
    if(spotlightBrightnessFL == INVALID_UNIFORM_LOCATION) {return false;}
    pointlightPosFL = GetUniformLocation("pointlightPos", shaderFL);
    if(pointlightPosFL == INVALID_UNIFORM_LOCATION) {return false;}
    pointlightBrightnessFL = GetUniformLocation("pointlightBrightness", shaderFL);
    
    if(pointlightBrightnessFL == INVALID_UNIFORM_LOCATION) {return false;}
    dirlightDirFL = GetUniformLocation("dirlightDir", shaderFL);
    if(dirlightDirFL == INVALID_UNIFORM_LOCATION) {return false;}
    dirlightBrightnessFL = GetUniformLocation("dirlightBrightness", shaderFL);
    if(dirlightBrightnessFL == INVALID_UNIFORM_LOCATION) {return false;}
    
    
    
    projMatVL = GetUniformLocation("projectionMatrix", shaderVL);
    if(projMatVL == INVALID_UNIFORM_LOCATION) {return false;}
    viewMatVL = GetUniformLocation("viewMatrix", shaderVL);
    if(viewMatVL == INVALID_UNIFORM_LOCATION) {return false;}
    modelMatVL = GetUniformLocation("modelMatrix", shaderVL);
    if(modelMatVL == INVALID_UNIFORM_LOCATION) {return false;}
    normalMatVL = GetUniformLocation("normalMatrix", shaderVL);
    if(normalMatVL == INVALID_UNIFORM_LOCATION) {return false;}
    
    texVL = GetUniformLocation("tex", shaderVL);
    if(texVL == INVALID_UNIFORM_LOCATION) {return false;}
    camPosVL = GetUniformLocation("camPos", shaderVL);
    if(camPosVL == INVALID_UNIFORM_LOCATION) {return false;}
    kaVL = GetUniformLocation("ka", shaderVL);
    if(kaVL == INVALID_UNIFORM_LOCATION) {return false;}
    kdVL = GetUniformLocation("kd", shaderVL);
    if(kdVL == INVALID_UNIFORM_LOCATION) {return false;}
    ksVL = GetUniformLocation("ks", shaderVL);
    if(ksVL == INVALID_UNIFORM_LOCATION) {return false;}
    shininessVL = GetUniformLocation("shininess", shaderVL);
    if(shininessVL == INVALID_UNIFORM_LOCATION) {return false;}
    
    spotlightPosVL = GetUniformLocation("spotlightPos", shaderVL);
    if(spotlightPosVL == INVALID_UNIFORM_LOCATION) {return false;}
    spotlightDirVL = GetUniformLocation("spotlightDir", shaderVL);
    if(spotlightDirVL == INVALID_UNIFORM_LOCATION) {return false;}
    spotlightFullVL = GetUniformLocation("spotlightFull", shaderVL);
    if(spotlightFullVL == INVALID_UNIFORM_LOCATION) {return false;}
    spotlightFadeVL = GetUniformLocation("spotlightFade", shaderVL);
    if(spotlightFadeVL == INVALID_UNIFORM_LOCATION) {return false;}
    spotlightBrightnessVL = GetUniformLocation("spotlightBrightness", shaderVL);
    if(spotlightBrightnessVL == INVALID_UNIFORM_LOCATION) {return false;}
    
    pointlightPosVL = GetUniformLocation("pointlightPos", shaderVL);
    if(pointlightPosVL == INVALID_UNIFORM_LOCATION) {return false;}
    pointlightBrightnessVL = GetUniformLocation("pointlightBrightness", shaderVL);
    if(pointlightBrightnessVL == INVALID_UNIFORM_LOCATION) {return false;}
    
    dirlightDirVL = GetUniformLocation("dirlightDir", shaderVL);
    if(dirlightDirVL == INVALID_UNIFORM_LOCATION) {return false;}
    dirlightBrightnessVL = GetUniformLocation("dirlightBrightness", shaderVL);
    if(dirlightBrightnessVL == INVALID_UNIFORM_LOCATION) {return false;}
    
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
    
    for(usi i = 0; i < 4; ++i) {
        vertexVectors.push_back(new std::vector<VertexData>());
        indexVectors.push_back(new std::vector<unsigned int>());
    }
    loadMesh("box.obj",         *(vertexVectors[0]), *(indexVectors[0]));
    loadMesh("cylinder.obj",    *(vertexVectors[1]), *(indexVectors[1]));
    loadMesh("sphere.obj",      *(vertexVectors[2]), *(indexVectors[2]));
    btTriangleMesh * trimesh0 = new btTriangleMesh();
    loadMesh("icosahedron.obj",*(vertexVectors[3]), *(indexVectors[3]), trimesh0);
    trimesh0->setScaling({1, 1, 1});
    for(usi i = 0; i < 3; ++i) {
        texes.push_back(GLuint(0));
    }
    //lol
    texes[0] = loadTexture("wood_texture.jpg");
    texes[1] = loadTexture("aluminum.jpg");
    texes[2] = loadTexture("blue_texture.jpg");
    
    collisionShapes.push_back(new btBoxShape({1.0f, 1.0f, 1.0f}));
    collisionShapes.push_back(new btCylinderShape({4.0f, 4.0f, 4.0f}));
    collisionShapes.push_back(new btSphereShape(1.0f));
    
    collisionShapes.push_back(new btBoxShape({20.0f, 1.0f, 20.0f}));
    collisionShapes.push_back(new btBoxShape({20.0f, 2.0f,  1.0f}));
    collisionShapes.push_back(new btBoxShape({ 1.0f, 2.0f, 18.0f}));
    collisionShapes.push_back(new btConvexTriangleMeshShape(trimesh0));
    
    btVector3 btZeroVec3(0, 0, 0);
    
    btRigidBody::btRigidBodyConstructionInfo CI(
        0,
        new btDefaultMotionState(btTransform(
            {0, 0, 0, 1},
            {-5, 2, 0}
        )),
        collisionShapes[0],
        btZeroVec3
    );
    CI.m_restitution = 0.2f;
    objects.push_back(new Object(
        vertexVectors[0],
        indexVectors[0],
        texes[1],
        {1.0f, 1.0f, 1.0f},
        dynamicsWorld,
        CI,
        true
    ));
    
    CI = btRigidBody::btRigidBodyConstructionInfo(
        0,
        new btDefaultMotionState(btTransform(
            {0, 0, 0, 1},
            {0, 2, 0}
        )),
        collisionShapes[1],
        btZeroVec3
    );
    CI.m_restitution = 4.0f;
    objects.push_back(new Object(
        vertexVectors[1],
        indexVectors[1],
        texes[2],
        {4.0f, 1.0f, 4.0f},
        dynamicsWorld,
        CI
    ));
    
    btVector3 inertia(0.0f, 0.0f, 0.0f);
    collisionShapes[2]->calculateLocalInertia(5.0f, inertia);
    CI = btRigidBody::btRigidBodyConstructionInfo(
        5,
        new btDefaultMotionState(btTransform(
            {0, 0, 0, 1},
            {-5, 2, 5}
        )),
        collisionShapes[2],
        inertia
    );
    CI.m_restitution = 1.0f;
    objects.push_back(new Object(
        vertexVectors[2],
        indexVectors[2],
        texes[1],
        {1.0f, 1.0f, 1.0f},
        dynamicsWorld,
        CI
    ));
    
    
    
    CI = btRigidBody::btRigidBodyConstructionInfo(
        0,
        new btDefaultMotionState(btTransform(
            {0, 0, 0, 1},
            {0, 0, 0}
        )),
        collisionShapes[3],
        btZeroVec3
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
            {0, 0, 0, 1},
            {0, 3, 19}
        )),
        collisionShapes[4],
        btZeroVec3
    );
    CI.m_restitution = 0.5f;
    objects.push_back(new Object(
        vertexVectors[0],
        indexVectors[0],
        texes[0],
        {20.0f, 2.0f, 1.0f},
        dynamicsWorld,
        CI
    ));
    
    CI = btRigidBody::btRigidBodyConstructionInfo(
        0,
        new btDefaultMotionState(btTransform(
            {0, 0, 0, 1},
            {0, 3, -19}
        )),
        collisionShapes[4],
        btZeroVec3
    );
    CI.m_restitution = 0.5f;
    objects.push_back(new Object(
        vertexVectors[0],
        indexVectors[0],
        texes[0],
        {20.0f, 2.0f, 1.0f},
        dynamicsWorld,
        CI
    ));
    
    CI = btRigidBody::btRigidBodyConstructionInfo(
        0,
        new btDefaultMotionState(btTransform(
            {0, 0, 0, 1},
            {19, 3, 0}
        )),
        collisionShapes[5],
        btZeroVec3
    );
    CI.m_restitution = 0.5f;
    objects.push_back(new Object(
        vertexVectors[0],
        indexVectors[0],
        texes[0],
        {1.0f, 2.0f, 18.0f},
        dynamicsWorld,
        CI
    ));
    
    CI = btRigidBody::btRigidBodyConstructionInfo(
        0,
        new btDefaultMotionState(btTransform(
            {0, 0, 0, 1},
            {-19, 3, 0}
        )),
        collisionShapes[5],
        btZeroVec3
    );
    CI.m_restitution = 0.5f;
    objects.push_back(new Object(
        vertexVectors[0],
        indexVectors[0],
        texes[0],
        {1.0f, 2.0f, 18.0f},
        dynamicsWorld,
        CI
    ));
    
    
    collisionShapes[6]->calculateLocalInertia(2.0f, inertia);
    CI = btRigidBody::btRigidBodyConstructionInfo(
        2,
        new btDefaultMotionState(btTransform(
            {0, 0, 0, 1},
            {5, 2, 5}
        )),
        collisionShapes[6],
        inertia
    );
    CI.m_restitution = 1.0f;
    objects.push_back(new Object(
        vertexVectors[3],
        indexVectors[3],
        texes[1],
        {1.0f, 1.0f, 1.0f},
        dynamicsWorld,
        CI
    ));
    
    
    return true;
}

void Graphics::moveCylinder(float x, float z) {
    btTransform trans;
    objects[0]->GetRigidBody()->getMotionState()->getWorldTransform(trans);
    x += trans.getOrigin().getX();
    z += trans.getOrigin().getZ();
    clip(x, -17, 17);
    clip(z, -17, 17);
    trans.setOrigin(btVector3(x, trans.getOrigin().getY(), z));
    objects[0]->GetRigidBody()->getMotionState()->setWorldTransform(trans);
}
void Graphics::increaseAL(float f) {
    for(Object * object : objects) {
        object->GetKa() += glm::vec3(f, f, f);
    }
}
void Graphics::increaseSL(float f) {
    objects[2]->GetKs() += glm::vec3(f, f, f);
}

void Graphics::Update(float dt) {
    moveCylinder(dx, dz);
    dx = dz = 0;
    dynamicsWorld->stepSimulation(dt, 10);
    for(Object * object : objects) {
        object->Update(dt);
    }
}

void Graphics::Render() {
    //Clear the screen
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Start the correct shader program
    if(lighting == 3) {
        shaderFL->Enable();
        projMat = projMatFL;
        viewMat = viewMatFL;
        modelMat = modelMatFL;
        normalMat = normalMatFL;
        tex = texFL;
        camPos = camPosFL;
        ka = kaFL;
        kd = kdFL;
        ks = ksFL;
        shininess = shininessFL;
        spotlightPos = spotlightPosFL;
        spotlightDir = spotlightDirFL;
        spotlightFull = spotlightFullFL;
        spotlightFade = spotlightFadeFL;
        spotlightBrightness = spotlightBrightnessFL;
        pointlightPos = pointlightPosFL;
        pointlightBrightness = pointlightBrightnessFL;
        dirlightDir = dirlightDirFL;
        dirlightBrightness = dirlightBrightnessFL;
        lighting = 1;
    } else if(lighting == 4) {
        shaderVL->Enable();
        projMat = projMatVL;
        viewMat = viewMatVL;
        modelMat = modelMatVL;
        normalMat = normalMatVL;
        tex = texVL;
        camPos = camPosVL;
        ka = kaVL;
        kd = kdVL;
        ks = ksVL;
        shininess = shininessVL;
        spotlightPos = spotlightPosVL;
        spotlightDir = spotlightDirVL;
        spotlightFull = spotlightFullVL;
        spotlightFade = spotlightFadeVL;
        spotlightBrightness = spotlightBrightnessVL;
        pointlightPos = pointlightPosVL;
        pointlightBrightness = pointlightBrightnessVL;
        dirlightDir = dirlightDirVL;
        dirlightBrightness = dirlightBrightnessVL;
        lighting = 2;
    }
    
    //Send in the projection and view to the shader
    glUniformMatrix4fv(projMat, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection())); 
    glUniformMatrix4fv(viewMat, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(tex, 0);
    glUniform3fv(camPos, 1, glm::value_ptr(m_camera->GetPosition()));
    
    btTransform sphereTrans;
    objects[2]->GetRigidBody()->getMotionState()->getWorldTransform(sphereTrans);
    btVector3 spherePos = sphereTrans.getOrigin();
    glUniform3fv(spotlightPos, 1, glm::value_ptr(glm::vec3(spherePos.getX(), spherePos.getY() + 20, spherePos.getZ())));
    glUniform3fv(spotlightDir, 1, glm::value_ptr(glm::vec3(0, -1, 0)));
    glUniform1f(spotlightFull, full);
    glUniform1f(spotlightFade, full-0.1f);
    glUniform3fv(spotlightBrightness, 1, glm::value_ptr(brightness*glm::vec3(cyanLight ? 0.5f : 1.f, 1.f, 1.f)));
    
    glUniform3fv(pointlightPos, 1, glm::value_ptr(glm::vec3(15.f, 5.f, 15.f)));
    glUniform3fv(pointlightBrightness, 1, glm::value_ptr(glm::vec3(15.f, 5.f, 0.f)));
    
    glUniform3fv(dirlightDir, 1, glm::value_ptr(glm::vec3(-1.f/3, -2.f/3, -2.f/3)));
    glUniform3fv(dirlightBrightness, 1, glm::value_ptr(glm::vec3(0.2f, 0.1f, 0.1f)));
    
    //Render the objects
    for(usi i = 0; i < objects.size(); ++i) {
        glUniformMatrix4fv(modelMat, 1, GL_FALSE, glm::value_ptr(objects[i]->GetModel()));
        glUniformMatrix4fv(normalMat, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(objects[i]->GetModel()))));
        glUniform3fv(ka, 1, glm::value_ptr(objects[i]->GetKa()));
        glUniform3fv(kd, 1, glm::value_ptr(objects[i]->GetKd()));
        glUniform3fv(ks, 1, glm::value_ptr(objects[i]->GetKs()));
        glUniform1f(shininess, objects[i]->GetShininess());
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

GLint Graphics::GetUniformLocation(std::string name, Shader * shader) const {
    GLint loc = shader->GetUniformLocation(name.c_str());
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
void loadMesh(std::string filename, std::vector<VertexData> & vertices, std::vector<unsigned int> & indices, btTriangleMesh * trimesh) {
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
            if(trimesh) {
                const glm::vec3 & v0 = vertices[face.mIndices[0]].position, & v1 = vertices[face.mIndices[1]].position, & v2 = vertices[face.mIndices[2]].position;
                trimesh->addTriangle({v0.x, v0.y, v0.z}, {v1.x, v1.y, v1.z}, {v2.x, v2.y, v2.z});
            }
        }
    }
}

