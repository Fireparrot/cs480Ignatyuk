#include "graphics.h"
#include <fstream>

using usi = unsigned short int;

float PI = 3.14159265358979323846264f;

void clip(float & x, float min, float max) {
    if(x < min) {x = min;}
    if(x > max) {x = max;}
}

Graphics::Graphics():
    lighting(3),
    dx(0),
    dy(0),
    dz(0),
    camTheta(0),
    camPhi(0),
    walkingType(0),
    crouchingAnimation(0),
    peek(0),
    peekingAnimation(0)
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

void checkUniform(std::string uniform, Shader * shader) {
    std::cout << uniform << ": " << shader->uniform(uniform) << std::endl;
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
    
    
    float roomWidth = 8.0f, roomLength = 12.f, roomHeight = 2.7f, wallWidth = 0.1f, rimHeight1 = 0.2f, rimHeight2 = 0.9f, cliffWidth = 3.5f, cliffHeight = 0.8f;
    
    for(usi i = 0; i < 7; ++i) {
        vertexVectors.push_back(new std::vector<VertexData>());
        indexVectors.push_back(new std::vector<unsigned int>());
    }
    loadMesh("sphere.obj", *(vertexVectors[0]), *(indexVectors[0]));
    btTriangleMesh * chairMesh = new btTriangleMesh();
    chairMesh->setScaling({0.3, 0.3, 0.3});
    loadMesh("desk3.obj",  *(vertexVectors[1]), *(indexVectors[1]), chairMesh);
    loadMesh("box.obj",    *(vertexVectors[2]), *(indexVectors[2]));
    
    
    for(usi i = 0; i < 7; ++i) {
        texes.push_back(GLuint(0));
    }
    texes[0] = loadTexture("wood_texture.jpg");
    texes[1] = loadTexture("seamless_wood1.jpg");
    texes[2] = loadTexture("desk2.png");
    texes[3] = loadTexture("white_wall.jpg");
    texes[4] = loadTexture("blue_carpet.jpg");
    texes[5] = loadTexture("blue_plastic.jpg");
    texes[6] = loadTexture("slide0.png");
    
    collisionShapes.push_back(new btCylinderShape({0.2f, 0.8f, 1.0f}));
    
    auto * chairShape = new btConvexTriangleMeshShape(chairMesh, true);
    btShapeHull* hull = new btShapeHull(chairShape);
	btScalar margin = chairShape->getMargin();
	hull->buildHull(margin);
	btConvexHullShape* simplifiedConvexShape = new btConvexHullShape((const btScalar *)(hull->getVertexPointer()),hull->numVertices());
    collisionShapes.push_back(simplifiedConvexShape);
    
    collisionShapes.push_back(new btBoxShape({roomLength/2, wallWidth/2, roomWidth/2}));
    
    collisionShapes.push_back(new btBoxShape({roomLength/2, roomHeight/2+cliffHeight/2, wallWidth/2}));
    collisionShapes.push_back(new btBoxShape({wallWidth/2, roomHeight/2+cliffHeight/2, roomWidth/2}));
    
    collisionShapes.push_back(new btBoxShape({roomLength/2, rimHeight1/2, wallWidth/4}));
    collisionShapes.push_back(new btBoxShape({wallWidth/4, rimHeight1/2, roomWidth/2}));
    
	collisionShapes.push_back(new btBoxShape({0.6f, 0.5f, 0.3f}));
	
	
	
    Object * player = makeObject({0, 1, 0, 0}, {-5, 1, 3}, {0, 0, 0}, 100, 0.1, 0.0, 0, 0, 0);
    player->rigidBody->setActivationState(DISABLE_DEACTIVATION);
    
    objects[0]->rigidBody->setAngularFactor({0, 0, 0});
    for(float x = -4.5f; x <= 4.5f; x += 1.5f) {
        for(float y = 1; y >= -3; --y) {
            makeObject({0, 0, 0, 1}, {x, 1, y}, {0.3, 0.3, 0.3}, 10, 0.1, 1.0, 1, 1, 2);
        }
    }
    
    makeObject({0, 0, 0, 1}, {2.5f, 0.5f,  2.5f}, {0.6f, 0.5f, 0.3f}, 0, 0.1, 1.0, 7, 2, 5);
    
    makeObject({0, 0, 0, 1}, {0, -wallWidth/2,  0}, {roomLength/2, wallWidth/2, roomWidth/2}, 0, 0.1, 1.0, 2, 2, 4);
    
    makeObject({0, 0, 0, 1}, {0,  roomHeight+wallWidth/2              ,  cliffWidth/2-roomWidth/2}, {roomLength/2, wallWidth/2  , cliffWidth/2            }, 0, 0.1, 1.0, 2, 2, 3);
    makeObject({0, 0, 0, 1}, {0,  roomHeight+wallWidth/2+cliffHeight/2,  cliffWidth-roomWidth/2  }, {roomLength/2, cliffHeight/2, wallWidth/2             }, 0, 0.1, 1.0, 2, 2, 3);
    makeObject({0, 0, 0, 1}, {0,  roomHeight+wallWidth/2+cliffHeight  ,  cliffWidth/2            }, {roomLength/2, wallWidth/2  , roomWidth/2-cliffWidth/2}, 0, 0.1, 1.0, 2, 2, 3);
    
    makeObject({0, 0, 0, 1}, {                        0,  roomHeight/2+cliffHeight/2,  roomWidth/2+wallWidth/2}, {roomLength/2, roomHeight/2+cliffHeight/2, wallWidth  }, 0, 0.1, 1.0, 3, 2, 3);
    makeObject({0, 0, 0, 1}, {                        0,  roomHeight/2+cliffHeight/2, -roomWidth/2-wallWidth/2}, {roomLength/2, roomHeight/2+cliffHeight/2, wallWidth  }, 0, 0.1, 1.0, 3, 2, 3);
    makeObject({0, 0, 0, 1}, { roomLength/2+wallWidth/2,  roomHeight/2+cliffHeight/2,                        0}, {wallWidth   , roomHeight/2+cliffHeight/2, roomWidth/2}, 0, 0.1, 1.0, 4, 2, 3);
    makeObject({0, 0, 0, 1}, {-roomLength/2-wallWidth/2,  roomHeight/2+cliffHeight/2,                        0}, {wallWidth   , roomHeight/2+cliffHeight/2, roomWidth/2}, 0, 0.1, 1.0, 4, 2, 3);
    
    makeObject({0, 0, 0, 1}, {                        0,  rimHeight2,  -roomWidth/2+wallWidth/2}, {roomLength/2, rimHeight1/2, wallWidth/4}, 0, 0.1, 1.0, 5, 2, 0);
    makeObject({0, 0, 0, 1}, { roomLength/2-wallWidth/2,  rimHeight2,                         0}, {wallWidth/4 , rimHeight1/2, roomWidth/2}, 0, 0.1, 1.0, 6, 2, 0);
    makeObject({0, 0, 0, 1}, {-roomLength/2+wallWidth/2,  rimHeight2,                         0}, {wallWidth/4 , rimHeight1/2, roomWidth/2}, 0, 0.1, 1.0, 6, 2, 0);
    
    std::cout << "lol" << std::endl;
    makeObject({0, roomHeight/2+cliffHeight/2, roomWidth/2-wallWidth/2*1.01f}, {1/sqrt(2.f), 0, 0, -1/sqrt(2.f)}, {1, 1.6f, 0.01f}, 2, 6);
    std::cout << "lol" << std::endl;
    
    checkUniform("projMat", shaderFL);
    checkUniform("viewMat", shaderFL);
    checkUniform("modelMat", shaderFL);
    checkUniform("normalMat", shaderFL);
    checkUniform("tex", shaderFL);
    checkUniform("camPos", shaderFL);
    checkUniform("ka", shaderFL);
    checkUniform("kd", shaderFL);
    checkUniform("ks", shaderFL);
    checkUniform("shininess", shaderFL);
    
    checkUniform("projMat", shaderVL);
    checkUniform("viewMat", shaderVL);
    checkUniform("modelMat", shaderVL);
    checkUniform("normalMat", shaderVL);
    checkUniform("tex", shaderVL);
    checkUniform("camPos", shaderVL);
    checkUniform("ka", shaderVL);
    checkUniform("kd", shaderVL);
    checkUniform("ks", shaderVL);
    checkUniform("shininess", shaderVL);
    
    return true;
}

Object * Graphics::makeObject(btQuaternion orientation, btVector3 position, glm::vec3 size, float mass, float restitution, float friction, usi shapeIndex, usi meshIndex, usi texIndex, bool isKinematic) {
    btVector3 inertia(0, 0, 0);
    if(mass != 0) {
        collisionShapes[shapeIndex]->calculateLocalInertia(mass, inertia);
    }
    btRigidBody::btRigidBodyConstructionInfo CI(
        mass,
        new btDefaultMotionState(btTransform(
            orientation,
            position
        )),
        collisionShapes[shapeIndex],
        inertia
    );
    CI.m_restitution = restitution;
    CI.m_friction = friction;
    objects.push_back(new Object(
        vertexVectors[meshIndex],
        indexVectors[meshIndex],
        texes[texIndex],
        size,
        dynamicsWorld,
        CI,
        isKinematic
    ));
    return objects.back();
}
Object * Graphics::makeObject(glm::vec3 pos, glm::quat rot, glm::vec3 size, usi meshIndex, usi texIndex) {
    objects.push_back(new Object(
        vertexVectors[meshIndex],
        indexVectors[meshIndex],
        texes[texIndex],
        pos,
        rot,
        size
    ));
    return objects.back();
}

float distance(float x0, float y0, float x1, float y1) {return sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));}

void Graphics::Update(float dt) {
    btTransform trans;
    objects[0]->rigidBody->getMotionState()->getWorldTransform(trans);
    bool onGround = trans.getOrigin().getY() < 0.9;
    
    float walkingSpeed = 1.0f;
    if(walkingType == -1) {
        walkingSpeed = 0.5f;
        crouchingAnimation += 20*(1.1f-crouchingAnimation)*(0.1f+crouchingAnimation)*dt;
        if(crouchingAnimation > 1.0f) {crouchingAnimation = 1.0f;}
    } else {
        crouchingAnimation -= 20*(1.1f-crouchingAnimation)*(0.1f+crouchingAnimation)*dt;
        if(crouchingAnimation < 0.0f) {crouchingAnimation = 0.0f;}
        if(walkingType == 1) {walkingSpeed = 3.0f;}
    }
    if(peek == -1) {
        peekingAnimation -= 10*(1.1f-peekingAnimation)*(1.1f+peekingAnimation)*dt;
        if(crouchingAnimation < -1.0f) {crouchingAnimation = -1.0f;}
    } else if(peek == 1) {
        peekingAnimation += 10*(1.1f-peekingAnimation)*(1.1f+peekingAnimation)*dt;
        if(crouchingAnimation > 1.0f) {crouchingAnimation = 1.0f;}
    } else {
        peekingAnimation -= 10*peekingAnimation*abs(peekingAnimation)*dt;
    }
    
    const btVector3 & v = objects[0]->rigidBody->getLinearVelocity();
    if(onGround) {
        objects[0]->rigidBody->setLinearVelocity({v.getX()*(1-4*dt), v.getY(), v.getZ()*(1-4*dt)});
    }
    
    objects[0]->rigidBody->activate(true);
    objects[0]->rigidBody->applyCentralForce({
            (sin(camTheta)*dz+cos(camTheta)*dx)*(onGround ? 1 : 0.3f)*walkingSpeed*(dy > 0 ? 2 : 1),
            dy*onGround*(walkingType == -1 ? 0 : 1),
            (cos(camTheta)*dz-sin(camTheta)*dx)*(onGround ? 1 : 0.3f)*walkingSpeed*(dy > 0 ? 2 : 1)});
    //std::cout << "y: " << trans.getOrigin().getY() << std::endl;
    //std::cout << "v: <" << v.getX() << ", " << v.getY() << ", " << v.getZ() << ">\n";
    
    dynamicsWorld->stepSimulation(dt, 10);
    for(Object * object : objects) {
        object->Update(dt);
    }
}

void Graphics::Render() {
    //Clear the screen
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    btTransform trans;
    objects[0]->rigidBody->getMotionState()->getWorldTransform(trans);
    glm::vec3 camPos = {trans.getOrigin().getX()-0.2f*peekingAnimation*cos(camTheta), trans.getOrigin().getY()+0.4f-0.8f*crouchingAnimation, trans.getOrigin().getZ()+0.2f*peekingAnimation*sin(camTheta)};
    m_camera->SetView(camPos, camPos + glm::vec3{sin(camTheta)*cos(camPhi), sin(camPhi), cos(camTheta)*cos(camPhi)});
    
    static Shader * shader;
    if(lighting == 3) {shader = shaderFL;}
    else if(lighting == 4) {shader = shaderVL;}
    if(lighting >= 3) {shader->Enable(); lighting -= 2;}
    
    //Send in the projection and view to the shader
    glUniformMatrix4fv(shader->uniform("projMat"), 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection())); 
    glUniformMatrix4fv(shader->uniform("viewMat"), 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(shader->uniform("tex"), 0);
    glUniform3fv(shader->uniform("camPos"), 1, glm::value_ptr(m_camera->GetPosition()));
    
    
    //Render the objects
    for(usi i = 0; i < objects.size(); ++i) {
        glUniformMatrix4fv(shader->uniform("modelMat"), 1, GL_FALSE, glm::value_ptr(objects[i]->GetModel()));
        glUniformMatrix4fv(shader->uniform("normalMat"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(objects[i]->GetModel()))));
        glUniform3fv(shader->uniform("ka"), 1, glm::value_ptr(objects[i]->GetKa()));
        glUniform3fv(shader->uniform("kd"), 1, glm::value_ptr(objects[i]->GetKd()));
        glUniform3fv(shader->uniform("ks"), 1, glm::value_ptr(objects[i]->GetKs()));
        glUniform1f(shader->uniform("shininess"), objects[i]->GetShininess());
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
    if(scene == 0) {std::cerr << "No file " << filename << " exists!" << std::endl;}
    
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
                //std::cout << "Adding: " << '{' << v0.x << ", " << v0.y << ", " << v0.z << "}, " << '{' << v1.x << ", " << v1.y << ", " << v1.z << "}, " << '{' << v2.x << ", " << v2.y << ", " << v2.z << "}" << std::endl;
            }
        }
    }
}

