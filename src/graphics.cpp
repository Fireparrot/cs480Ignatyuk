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
    brightness(300.f),
    full(0.9f),
    cyanLight(false),
    plungerHeld(false),
    flipperLeftHeld(false),
    flipperRightHeld(false),
    lives(0),
    scoreCode(0),
    addBallActive(true)
{}

Graphics::~Graphics() {
    //toptenFile.close();
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
    
    bumpedFL = GetUniformLocation("bumped", shaderFL);
    if(bumpedFL == INVALID_UNIFORM_LOCATION) {return false;}
    
    
    
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
    
    bumpedVL = GetUniformLocation("bumped", shaderVL);
    if(bumpedVL == INVALID_UNIFORM_LOCATION) {return false;}
    
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

    dynamicsWorld->setGravity(btVector3(0, -10, 6));
    
    for(usi i = 0; i < 7; ++i) {
        vertexVectors.push_back(new std::vector<VertexData>());
        indexVectors.push_back(new std::vector<unsigned int>());
    }
    loadMesh("box.obj",         *(vertexVectors[0]), *(indexVectors[0]));
    loadMesh("cylinder.obj",    *(vertexVectors[1]), *(indexVectors[1]));
    loadMesh("sphere.obj",      *(vertexVectors[2]), *(indexVectors[2]));
    btTriangleMesh * trimesh0 = new btTriangleMesh();
    loadMesh("icosahedron.obj", *(vertexVectors[3]), *(indexVectors[3]), trimesh0);
    trimesh0->setScaling({1, 1, 1});
    btTriangleMesh * trimesh1 = new btTriangleMesh();
    loadMesh("flipper.obj",     *(vertexVectors[4]), *(indexVectors[4]), trimesh1);
    trimesh1->setScaling({1, 1, 1});
    btTriangleMesh * trimesh2 = new btTriangleMesh();
    loadMesh("triangular_prism.obj",     *(vertexVectors[5]), *(indexVectors[5]), trimesh2);
    trimesh2->setScaling({2, 1, 2});
    
    for(usi i = 0; i < 4; ++i) {
        texes.push_back(GLuint(0));
    }
    texes[0] = loadTexture("wood_texture.jpg");
    texes[1] = loadTexture("aluminum.jpg");
    texes[2] = loadTexture("blue_texture.jpg");
    texes[3] = loadTexture("backsplash.png");
    
    collisionShapes.push_back(new btBoxShape({1.0f, 1.0f, 0.5f}));
    collisionShapes.push_back(new btCylinderShape({1.0f, 1.0f, 1.0f}));
    collisionShapes.push_back(new btSphereShape(1.0f));
    
    collisionShapes.push_back(new btBoxShape({20.0f, 1.0f, 20.0f}));
    collisionShapes.push_back(new btBoxShape({20.0f, 2.0f,  1.0f}));
    collisionShapes.push_back(new btBoxShape({ 1.0f, 2.0f, 18.0f}));
    collisionShapes.push_back(new btBoxShape({ 5.0f, 1.0f,  1.0f}));
    collisionShapes.push_back(new btBvhTriangleMeshShape(trimesh0, true));
    collisionShapes.push_back(new btBvhTriangleMeshShape(trimesh1, true));
    collisionShapes.push_back(new btBvhTriangleMeshShape(trimesh2, true));
    collisionShapes.push_back(new btBoxShape({ 0.4f, 2.0f, 10.0f}));
    collisionShapes.push_back(new btBoxShape({20.0f, 1.0f,  5.0f}));
    
    btQuaternion noRotation(0, 0, 0, 1);
    btVector3 btZeroVec3(0, 0, 0);
    glm::vec3 standardSize(1, 1, 1);
    
    makeObject(noRotation, {17, 2, 17}, {1.f, 1.f, 0.5f}, 0, 0.2f, 0, 0, 1, true);
    makeObject(noRotation, {-6, 2, 12}, standardSize, 0, 0.5f, 6, 4, 1, true);
    makeObject(noRotation, { 6, 2, 12}, standardSize, 0, 0.5f, 6, 4, 1, true);
    
    makeObject(noRotation, {8.f, 2, 3}, standardSize, 5.0f, 1.0f, 2, 2, 1);
    makeObject(noRotation, {0, 2, 5}, standardSize, 2.f, 1.0f, 7, 3, 1);
    
    makeObject(noRotation, {0, 0, 0}, {20.f, 1.f, 20.f}, 0, 0, 3, 0, 0);
    makeObject(noRotation, {0, 3,  19}, {20.f, 2.f, 1.f}, 0, 0.5f, 4, 0, 0);
    makeObject(noRotation, {0, 3, -19}, {20.f, 2.f, 1.f}, 0, 0.5f, 4, 0, 0);
    makeObject(noRotation, { 19, 3, 0}, {1.f, 2.f, 18.f}, 0, 0.5f, 5, 0, 0);
    makeObject(noRotation, {-19, 3, 0}, {1.f, 2.f, 18.f}, 0, 0.5f, 5, 0, 0);
    
    makeObject({{1, 0, 0}, PI/6}, {0, 5, -25}, {20.f, 1.f, 5.f}, 0, 0, 11, 0, 0);
    
    makeObject(noRotation, { 15.5f, 2, 8}, {0.4f, 1.f, 10.f}, 0, 0.5f, 10, 0, 0);
    makeObject({0,  sin(PI/8), 0, cos(PI/8)}, { 12, 2, -12}, {0.4f, 1.f, 10.f}, 0, 0.5f, 10, 0, 0);
    makeObject({0, -sin(PI/8), 0, cos(PI/8)}, {-12, 2, -12}, {0.4f, 1.f, 10.f}, 0, 0.5f, 10, 0, 0);
    
    makeObject(noRotation, { 10, 2, 8.9f}, {2, 1, 2}, 0, 0.5f, 9, 5, 1);
    makeObject({{0, 0, 1}, PI}, {-10, 2, 8.9f}, {2, 1, 2}, 0, 0.5f, 9, 5, 1);
    
    makeObject(noRotation, { 0, 2, -12}, standardSize, 0, 2.0f, 1, 1, 2);
    makeObject(noRotation, { 3, 2, -9}, standardSize, 0, 2.0f, 1, 1, 2);
    makeObject(noRotation, {-3, 2, -9}, standardSize, 0, 2.0f, 1, 1, 2);
    
    /*
    std::ifstream playerFile;
    playerFile.open("player.txt");
    if(playerFile.is_open()) {
        while(playerFile.peek() != '\n' && !playerFile.eof()) {
            playerName += playerFile.get();
        }
    } else {playerName = "Bob";}
    std::cout << "Playing as " << playerName << "!" << std::endl;*/
    
    resetLife();
    deactivateAddBall();
    
    toptenFile.open("topten.txt");
    
    return true;
}

void Graphics::makeObject(btQuaternion orientation, btVector3 position, glm::vec3 size, float mass, float restitution, usi shapeIndex, usi meshIndex, usi texIndex, bool isKinematic) {
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
    objects.push_back(new Object(
        vertexVectors[meshIndex],
        indexVectors[meshIndex],
        texes[texIndex],
        size,
        dynamicsWorld,
        CI,
        isKinematic
    ));
}

void Graphics::resetLife() {
    if(lives <= 1) {
        lives = 3;
        score = 0;
        /*
        topten.clear();
        toptenFile.clear();
        toptenFile.seekg(0, ios::beg);
        while(!toptenFile.eof()) {
            std::string name;
            toptenFile >> name;
            float score_;
            toptenFile >> score_;
            topten.push_back({name, score_});
        }
        int i = 0;
        while(i < topten.size() && score < topten[i].second) {++i;}
        topten.insert(topten.begin()+i, {playerName, score});
        if(topten.size() > 10) {topten.erase(topten.begin()+10);}*/
    } else {
        --lives;
    }
    btTransform trans;
    trans.setOrigin({17.f, 2.f, plungerHeight-2.0f});
    objects[3]->GetRigidBody()->setWorldTransform(trans);
    objects[3]->GetRigidBody()->setLinearVelocity({0, 0, 0});
    objects[3]->GetRigidBody()->setAngularVelocity({0, 0, 0});
    std::cout << "Lives: " << lives << std::endl;
    std::cout << "Score: " << score << std::endl;
}
void Graphics::deactivateAddBall() {
    if(addBallActive) {
        btTransform trans;
        trans.setOrigin({17.f, -2.f, 0});
        objects[4]->GetRigidBody()->getMotionState()->setWorldTransform(trans);
        dynamicsWorld->removeRigidBody(objects[4]->GetRigidBody());
    }
}
void Graphics::activateAddBall() {
    if(!addBallActive) {
        dynamicsWorld->addRigidBody(objects[4]->GetRigidBody());
        btTransform trans;
        trans.setOrigin({17.f, 2.f, plungerHeight-4.0f});
        objects[4]->GetRigidBody()->setWorldTransform(trans);
        objects[4]->GetRigidBody()->setLinearVelocity({0, 0, 0});
        objects[4]->GetRigidBody()->setAngularVelocity({0, 0, 0});
    }
}

void Graphics::increaseAL(float f) {
    for(Object * object : objects) {
        object->GetKa() += glm::vec3(f, f, f);
    }
}
void Graphics::increaseSL(float f) {
    objects[3]->GetKs() += glm::vec3(f, f, f);
}

float distance(float x0, float y0, float x1, float y1) {return sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));}

void Graphics::Update(float dt) {
    
    score += dt;
    if(scoreCode == 1) {score += 10; scoreCode = 0; std::cout << "Score: " << score << std::endl;}
    else if(scoreCode == 2) {score += 4; scoreCode = 0; std::cout << "Score: " << score << std::endl;}
    
    if(plungerHeld) {
        plungerHeight += dt*3;
    } else {
        plungerHeight -= dt*20;
    }
    clip(plungerHeight, 15, 17);
    
    if(flipperLeftHeld) {
        flipperLeftRot += dt*7;
    } else {
        flipperLeftRot -= dt*7;
    }
    clip(flipperLeftRot, -PI/6, PI/6);
    
    if(flipperRightHeld) {
        flipperRightRot -= dt*7;
    } else {
        flipperRightRot += dt*7;
    }
    clip(flipperRightRot, 5*PI/6, 7*PI/6);
    
    if(cameraMove == 0) {cameraTheta -= cameraTheta*dt*5;}
    else {cameraTheta += cameraMove*dt*(PI/6-abs(cameraTheta))*5;}
    m_camera->SetPosition({20*sin(cameraTheta), 35.f, 20*cos(cameraTheta)});
    
    
    btTransform trans;
    objects[0]->GetRigidBody()->getMotionState()->getWorldTransform(trans);
    trans.setOrigin({trans.getOrigin().getX(), trans.getOrigin().getY(), plungerHeight});
    objects[0]->GetRigidBody()->getMotionState()->setWorldTransform(trans);
    
    objects[1]->GetRigidBody()->getMotionState()->getWorldTransform(trans);
    trans.setRotation({0, sin(flipperLeftRot/2), 0, cos(flipperLeftRot/2)});
    objects[1]->GetRigidBody()->getMotionState()->setWorldTransform(trans);
    
    objects[2]->GetRigidBody()->getMotionState()->getWorldTransform(trans);
    trans.setRotation({0, sin(flipperRightRot/2), 0, cos(flipperRightRot/2)});
    objects[2]->GetRigidBody()->getMotionState()->setWorldTransform(trans);
    
    
    objects[3]->GetRigidBody()->getMotionState()->getWorldTransform(trans);
    float ballx = trans.getOrigin().getX(), bally = trans.getOrigin().getY(), ballz = trans.getOrigin().getZ();
    
    if(distance(ballx, ballz, 0, -12) < 2.3f) {
        if(scoreCode == 0) {scoreCode = -1;}
        bumperBumpCenter = 0.4f;
    } else {
        if(scoreCode == -1) {scoreCode = 1;}
    }
    
    if(distance(ballx, ballz,  3, -9) < 2.3f) {
        if(scoreCode == 0) {scoreCode = -1;}
        bumperBumpRight = 0.4f;
    } else {
        if(scoreCode == -1) {scoreCode = 1;}
    }
    
    if(distance(ballx, ballz, -3, -9) < 2.3f) {
        if(scoreCode == 0) {scoreCode = -1;}
        bumperBumpLeft = 0.4f;
    } else {
        if(scoreCode == -1) {scoreCode = 1;}
    }
    
    if(ballx < 13 && ballx > 7 && ballx+2*ballz > 25) {
        if(scoreCode == 0) {scoreCode = -2;}
        padBumpRight = 0.4f;
    } else {
        if(scoreCode == -2) {scoreCode = 2;}
    }
    
    if(ballx < -7 && ballx > -13 && -ballx+2*ballz > 25) {
        if(scoreCode == 0) {scoreCode = -2;}
        padBumpLeft = 0.4f;
    } else {
        if(scoreCode == -2) {scoreCode = 2;}
    }
    
    if(bumperBumpCenter > 0) {bumperBumpCenter -= dt;}
    if(bumperBumpLeft   > 0) {bumperBumpLeft   -= dt;}
    if(bumperBumpRight  > 0) {bumperBumpRight  -= dt;}
    if(padBumpRight     > 0) {padBumpRight     -= dt;}
    if(padBumpLeft      > 0) {padBumpLeft      -= dt;}
    
    if(ballz > 16.8f || bally < 0) {
        resetLife();
    }
    
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
        bumped = bumpedFL;
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
        bumped = bumpedVL;
        lighting = 2;
    }
    
    //Send in the projection and view to the shader
    glUniformMatrix4fv(projMat, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection())); 
    glUniformMatrix4fv(viewMat, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(tex, 0);
    glUniform3fv(camPos, 1, glm::value_ptr(m_camera->GetPosition()));
    
    btTransform sphereTrans;
    objects[3]->GetRigidBody()->getMotionState()->getWorldTransform(sphereTrans);
    btVector3 spherePos = sphereTrans.getOrigin();
    glUniform3fv(spotlightPos, 1, glm::value_ptr(glm::vec3(spherePos.getX(), spherePos.getY() + 20, spherePos.getZ())));
    glUniform3fv(spotlightDir, 1, glm::value_ptr(glm::vec3(0, -1, 0)));
    glUniform1f(spotlightFull, full);
    glUniform1f(spotlightFade, full-0.1f);
    glUniform3fv(spotlightBrightness, 1, glm::value_ptr(brightness*glm::vec3(cyanLight ? 0.5f : 1.f, 1.f, 1.f)));
    
    glUniform3fv(pointlightPos, 1, glm::value_ptr(glm::vec3(0.f, 5.f, -12.f)));
    glUniform3fv(pointlightBrightness, 1, glm::value_ptr(glm::vec3(15.f, 5.f, 0.f)));
    
    glUniform3fv(dirlightDir, 1, glm::value_ptr(glm::vec3(-1.f/3, -2.f/3, -2.f/3)));
    glUniform3fv(dirlightBrightness, 1, glm::value_ptr(glm::vec3(0.4f, 0.4f, 0.4f)));
    
    //Render the objects
    for(usi i = 0; i < objects.size(); ++i) {
        switch(i) {
            case(14):
                glUniform1i(bumped, padBumpRight > 0 ? 1 : 0);
                break;
            case(15):
                glUniform1i(bumped, padBumpLeft > 0 ? 1 : 0);
                break;
                
            case(16):
                glUniform1i(bumped, bumperBumpCenter > 0 ? 1 : 0);
                break;
            case(17):
                glUniform1i(bumped, bumperBumpRight > 0 ? 1 : 0);
                break;
            case(18):
                glUniform1i(bumped, bumperBumpLeft > 0 ? 1 : 0);
                break;
                
            default:
                glUniform1i(bumped, 0);
        }
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
                //std::cout << "Adding: " << '{' << v0.x << ", " << v0.y << ", " << v0.z << "}, " << '{' << v1.x << ", " << v1.y << ", " << v1.z << "}, " << '{' << v2.x << ", " << v2.y << ", " << v2.z << "}" << std::endl;
            }
        }
    }
}

