#include "graphics.h"
#include "menu.h"
#include <fstream>

#include <glm/ext.hpp>
#include <FTGL/ftgl.h>

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
    camTheta(-PI),
    camPhi(0),
    walkingType(0),
    crouchingAnimation(0),
    peek(0),
    peekingAnimation(0),
    roomBrightness(1, 1, 1),
    pauseGame(false),
    paintRadius(0.03f),
    paintColor{0, 0, 1, 1},
    paint(false),
    hasPrev(false),
    interactionStage(0),
    doorLeftA(0),
    doorRightA(0),
    doorLeftC(false),
    doorRightC(false),
    lightsOn(true),
    flashOn(0),
    hasBall(true),
    throwBall(false)
{}

Graphics::~Graphics() {
    delete menu;
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
    
    ih = new ImageHelper();
    
    ih->setPadding(25, 25, 10, 10);
    ih->setMin(300, 100);
    ih->setUsePadding(false);
    ih->setForeColor(1, 1, 0, 1);
    ih->setBackColor(0, 0, 1, 0);
    ih->setFontSize(40);
    ih->setFont("truetype/freefont/FreeSarif.ttf");
    
    menu = new Menu(this, "shaderMenu.vert", "shaderMenu.frag");
    
    GLint menuTex, menuHoverTex, menuClickTex, menuSliderTex, lowGravTex, highGravTex;
    lowGravTex = ih->rasterizeText("grav: low");
    highGravTex = ih->rasterizeText("grav: high");
    menuTex = ih->loadTexture("menu.png");
    menuHoverTex = ih->loadTexture("menu_hover.png");
    menuClickTex = ih->loadTexture("menu_click.png");
    menuSliderTex = ih->loadTexture("menu_slider.png");
    
    menu->add({-800, 400}, {-500, 500}, 0.f,
            ih->rasterizeText("menu"), menuTex, menuHoverTex, menuClickTex,
            [] (Menu * menu, MenuItem * item, float x, float y) {menu->activeID = 1;});
    
    
    
    menu->add({-150, 150}, { 150, 250}, 0.f,
            lowGravTex, menuTex, menuHoverTex, menuClickTex,
            [=] (Menu * menu, MenuItem * item, float x, float y) {
                    static bool g = false;
                    g = !g; menu->graphics->dynamicsWorld->setGravity({0, g ? -20.f : -10.f, 0});
                    item->overlayTexture = g ? highGravTex : lowGravTex;});
            
    menu->add({-150,-100}, { 150, 000}, 0.f,
            ih->rasterizeText("back"), menuTex, menuHoverTex, menuClickTex,
            [] (Menu * menu, MenuItem * item, float x, float y) {menu->activeID = 0;});
            
    menu->add({-150,-250}, { 150,-150}, 0.f,
            ih->rasterizeText("exit"), menuTex, menuHoverTex, menuClickTex,
            [] (Menu * menu, MenuItem * item, float x, float y) {menu->exitRequest = true;});
    
    
    ih->setFontSize(60);
    ih->setForeColor(1, 0, 0, 1);
    menu->add({-500, 250}, {-200, 350}, 0.f,
            ih->rasterizeText(std::wstring()+wchar_t(0x2600)), menuSliderTex, menuSliderTex, menuSliderTex,
            [&] (Menu * menu, MenuItem * item, float x, float y) {setSlider(item, x, y); roomBrightness.r = 2*item->slider;},
            false, 0.5f, -0.5f, 0.5f);
            
    ih->setForeColor(0, 1, 0, 1);
    menu->add({-500, 150}, {-200, 250}, 0.f,
            ih->rasterizeText(std::wstring()+wchar_t(0x2600)), menuSliderTex, menuSliderTex, menuSliderTex,
            [&] (Menu * menu, MenuItem * item, float x, float y) {setSlider(item, x, y); roomBrightness.g = 2*item->slider;},
            false, 0.5f, -0.5f, 0.5f);
    
    ih->setForeColor(0, 0, 1, 1);
    menu->add({-500,  50}, {-200, 150}, 0.f,
            ih->rasterizeText(std::wstring()+wchar_t(0x2600)), menuSliderTex, menuSliderTex, menuSliderTex,
            [&] (Menu * menu, MenuItem * item, float x, float y) {setSlider(item, x, y); roomBrightness.b = 2*item->slider;},
            false, 0.5f, -0.5f, 0.5f);
            
    menu->addGroup({0});
    menu->addGroup({1, 2, 3, 4, 5, 6});
    
    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    //Enable alpha-test
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //Culls (i.e. doesn't render) back faces from triangles
    glEnable(GL_CULL_FACE);
    
    
    
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0, -10, 0));
    
    
    float roomWidth = 8.0f, roomLength = 12.f, roomHeight = 2.7f, wallWidth = 0.1f, rimHeight1 = 0.2f, rimHeight2 = 0.9f, cliffWidth = 3.5f, cliffHeight = 0.8f;
    float doorWidth = 1.2f, doorHeight = 2.2f, doorOffset = 1.6f;
    
    
    interacter = new Interacter();
    
    doorLeftI  = interacter->add({ roomLength/2-1, doorHeight/2, roomWidth/2+wallWidth/2}, doorWidth/2, 2.0f, {0, 0, 0},
            [&] () {
                doorLeftC = !doorLeftC;
            });
    doorRightI = interacter->add({-roomLength/2+1, doorHeight/2, roomWidth/2+wallWidth/2}, doorWidth/2, 2.0f, {0, 0, 0},
            [&] () {
                doorRightC = !doorRightC;
            });
    interacter->add({roomLength/2+wallWidth/2, doorHeight/2, -roomWidth/2+1}, doorWidth/2, 2.0f, {roomLength/2-wallWidth/2, doorHeight/2, -roomWidth/2+1-doorWidth/2+0.2f}, [] () {}, false);
    interacter->add({-roomLength/2+1.8f, roomHeight/2, roomWidth/2}, 0.1f, 2.0f, {-roomLength/2+1.8f, roomHeight/2, roomWidth/2-0.1f}, [&] () {lightsOn = !lightsOn;});
    ballI = interacter->add({0, 0, 0}, 0.3f, 2.f, {0, 0, 0},
            [&] () {
                hasBall = true;
            });
    
    for(usi i = 0; i < 10; ++i) {
        vertexVectors.push_back(new std::vector<VertexData>());
        indexVectors.push_back(new std::vector<unsigned int>());
    }
    loadMesh("sphere.obj", *(vertexVectors[0]), *(indexVectors[0]));
    btTriangleMesh * chairMesh = new btTriangleMesh();
    chairMesh->setScaling({0.3f, 0.3f, 0.3f});
    loadMesh("desk3.obj",  *(vertexVectors[1]), *(indexVectors[1]), chairMesh);
    loadMesh("box.obj",    *(vertexVectors[2]), *(indexVectors[2]));
    btTriangleMesh * podiumMesh = new btTriangleMesh();
    podiumMesh->setScaling({0.4f, 0.4f, 0.4f});
    loadMesh("teachers_desk2.obj", *(vertexVectors[3]), *(indexVectors[3]), podiumMesh);
    loadMesh("projector.obj", *(vertexVectors[4]), *(indexVectors[4]));
    loadMesh("harris2.obj", *(vertexVectors[5]), *(indexVectors[5]));
    loadMesh("doorframe.obj", *(vertexVectors[6]), *(indexVectors[6]));
    btTriangleMesh * doorMesh = new btTriangleMesh();
    loadMesh("door.obj", *(vertexVectors[7]), *(indexVectors[7]), doorMesh);
    
    
    for(usi i = 0; i < 20; ++i) {
        texes.push_back(GLuint(0));
    }
    texes[0] = ih->loadTexture("wood_texture.jpg");
    texes[1] = ih->loadTexture("door_texture.jpg");
    texes[2] = ih->loadTexture("desk3.png");
    texes[3] = ih->loadTexture("white_wall.jpg");
    texes[4] = ih->loadTexture("carpet_texture.jpg");
    texes[5] = ih->loadTexture("teachers_desk_texture.png");
    texes[6] = ih->loadTexture("projector_texture.jpg");
    texes[7] = ih->loadTexture("harris_texture.png");
    texes[8] = ih->loadTexture("blue_texture.jpg");
    
    ih->setBackColor(1, 1, 1, 1);
    ih->setForeColor(0, 0, 0, 1);
    ih->setMin((roomLength/2-2)*600, 1*600);
    ih->setFontSize(320);
    whiteboardTexture = texes[9] = ih->rasterizeText(std::wstring()+wchar_t(0x2600), &whiteboardImage);
    
    texes[10] = ih->loadTexture("slide0.png");
    
    collisionShapes.push_back(new btCylinderShape({0.2f, 0.8f, 1.0f}));
    
    auto * chairShape = new btConvexTriangleMeshShape(chairMesh, true);
    btShapeHull * chairHull = new btShapeHull(chairShape);
	btScalar margin = chairShape->getMargin();
	chairHull->buildHull(margin);
	btConvexHullShape * simplifiedChairShape = new btConvexHullShape((const btScalar *)(chairHull->getVertexPointer()), chairHull->numVertices());
    collisionShapes.push_back(simplifiedChairShape);
    
    collisionShapes.push_back(new btBoxShape({roomLength/2, wallWidth/2, roomWidth/2}));
    
    collisionShapes.push_back(new btBoxShape({roomLength/2, roomHeight/2+cliffHeight/2, wallWidth/2}));
    collisionShapes.push_back(new btBoxShape({wallWidth/2, roomHeight/2+cliffHeight/2, roomWidth/2}));
    
    collisionShapes.push_back(new btBoxShape({roomLength/2, rimHeight1/2, wallWidth/4}));
    collisionShapes.push_back(new btBoxShape({wallWidth/4, rimHeight1/2, roomWidth/2}));
    
    collisionShapes.push_back(new btSphereShape(0.3f));
    
    auto * podiumShape = new btConvexTriangleMeshShape(podiumMesh, true);
    btShapeHull * podiumHull = new btShapeHull(podiumShape);
	margin = podiumShape->getMargin();
	podiumHull->buildHull(margin);
	btConvexHullShape * simplifiedPodiumShape = new btConvexHullShape((const btScalar *)(podiumHull->getVertexPointer()), podiumHull->numVertices());
    collisionShapes.push_back(simplifiedPodiumShape);
    
	//9+
    collisionShapes.push_back(new btBoxShape({roomLength/2-doorOffset, roomHeight/2+cliffHeight/2, wallWidth/2}));
    collisionShapes.push_back(new btBoxShape({doorWidth/2, roomHeight/2+cliffHeight/2-doorHeight/2, wallWidth/2}));
    collisionShapes.push_back(new btBoxShape({doorOffset/2-doorWidth/2, roomHeight/2+cliffHeight/2, wallWidth/2}));
	
	auto * doorShape = new btConvexTriangleMeshShape(doorMesh, true);
    btShapeHull * doorHull = new btShapeHull(doorShape);
	margin = doorShape->getMargin();
	doorHull->buildHull(margin);
	btConvexHullShape * simplifiedDoorShape = new btConvexHullShape((const btScalar *)(doorHull->getVertexPointer()), doorHull->numVertices());
    collisionShapes.push_back(simplifiedDoorShape);
	
    user = makeObject({0, 1, 0, 0}, {-5, 1, 3}, {0, 0, 0}, 100, 0.1, 0.0, 0, 0, 0);
    user->rigidBody->setActivationState(DISABLE_DEACTIVATION);
    user->rigidBody->setAngularFactor({0, 0, 0});
    
    for(float x = -4.5f; x <= 4.5f; x += 1.5f) {
        for(float y = 1; y >= -3; --y) {
            makeObject({0, 0, 0, 1}, {x, 0, y}, {0.3, 0.3, 0.3}, 10, 0.1, 1.0, 1, 1, 2);
        }
    }
    
    whiteboard = makeObject({0, roomHeight/2+cliffHeight/2, roomWidth/2-wallWidth/2-0.01f}, {1/sqrt(2.f), 0, 0, -1/sqrt(2.f)}, {1, roomLength/2-2, 0.01f}, 2, 9);
    whiteboard->getKs() = {1.5f, 1.5f, 1.5f};
    
    slides = makeObject({0, roomHeight/2+cliffHeight+0.4f, roomWidth/2-wallWidth/2-0.3f}, {1/sqrt(2.f), 0, 0, -1/sqrt(2.f)}, {1, 1.6f, 0.01f}, 2, 10);
    
    ball = makeObject({0, 0, 0, 1}, {0, -2, 0}, {0.3f, 0.3f, 0.3f}, 10, 0.1, 0.2f, 7, 0, 0);
    ball->rigidBody->setActivationState(DISABLE_DEACTIVATION);
    
    //Light button
    makeObject({-roomLength/2+1.8f, roomHeight/2, roomWidth/2}, {1, 0, 0, 0}, {0.05f, 0.1f, 0.1f}, 2, 8);
    
    //Teacher's desk
    makeObject({0, -1/sqrt(2.f), 0, 1/sqrt(2.f)}, {2.5f, 0.5f,  2.5f}, {0.4f, 0.4f, 0.4f}, 0, 0.1, 1.0, 8, 3, 5);
    
    //Projector
    makeObject({0, roomHeight+cliffHeight/2-wallWidth, 0}, {1/sqrt(2.f), 0, -1/sqrt(2.f), 0}, {0.5f, 0.5f, 0.5f}, 4, 6);
    
    //Harris
    makeObject({2.5f, 0.8f, 3.3f}, {0, 0, 1, 0}, {0.8f, 0.8f, 0.8f}, 5, 7);
    
    //Door frames
    makeObject({ roomLength/2-1, 1, roomWidth/2}, {1, 0, 0, 0}, {1, 1, 1}, 6, 0);
    makeObject({-roomLength/2+1, 1, roomWidth/2}, {1, 0, 0, 0}, {1, 1, 1}, 6, 0);
    makeObject({roomLength/2, 1, -roomWidth/2+1}, {1/sqrt(2.f), 0, 1/sqrt(2.f), 0}, {1, 1, 1}, 6, 0);
    
    //Doors
    doorLeft  = makeObject({0, 1/sqrt(2.f), 0, 1/sqrt(2.f)}, { roomLength/2-1, doorHeight/2, roomWidth/2+wallWidth/2}, {1, 1, 1}, 0, 0.1, 1.0, 12, 7, 1, true);
    doorRight = makeObject({0, 1/sqrt(2.f), 0, 1/sqrt(2.f)}, {-roomLength/2+1, doorHeight/2, roomWidth/2+wallWidth/2}, {1, 1, 1}, 0, 0.1, 1.0, 12, 7, 1, true);
    makeObject({roomLength/2+wallWidth/2-0.01f, doorHeight/2, -roomWidth/2+1}, {1, 0, 0, 0}, {1, 1, 1}, 7, 1);
    
    //Floor
    makeObject({0, 0, 0, 1}, {0, -wallWidth/2,  0}, {roomLength/2, wallWidth/2, roomWidth/2}, 0, 0.1, 1.0, 2, 2, 4);
    
    //Ceiling+cliff
    makeObject({0, 0, 0, 1}, {0,  roomHeight+wallWidth/2              ,  cliffWidth/2-roomWidth/2}, {roomLength/2, wallWidth/2  , cliffWidth/2            }, 0, 0.1, 1.0, 2, 2, 3);
    makeObject({0, 0, 0, 1}, {0,  roomHeight+wallWidth/2+cliffHeight/2,  cliffWidth-roomWidth/2  }, {roomLength/2, cliffHeight/2, wallWidth/2             }, 0, 0.1, 1.0, 2, 2, 3);
    makeObject({0, 0, 0, 1}, {0,  roomHeight+wallWidth/2+cliffHeight  ,  cliffWidth/2            }, {roomLength/2, wallWidth/2  , roomWidth/2-cliffWidth/2}, 0, 0.1, 1.0, 2, 2, 3);
    
    //Walls
    makeObject({0, 0, 0, 1},
            {0,  roomHeight/2+cliffHeight/2, roomWidth/2+wallWidth/2},
            {roomLength/2-doorOffset, roomHeight/2+cliffHeight/2, wallWidth/2},
            0, 0.1, 1.0, 9, 2, 3);
    makeObject({0, 0, 0, 1},
            { roomLength/2-1, roomHeight/2+cliffHeight/2+doorHeight/2,  roomWidth/2+wallWidth/2},
            {doorWidth/2, roomHeight/2+cliffHeight/2-doorHeight/2, wallWidth/2},
            0, 0.1, 1.0, 10, 2, 3);
    makeObject({0, 0, 0, 1},
            {-roomLength/2+1, roomHeight/2+cliffHeight/2+doorHeight/2,  roomWidth/2+wallWidth/2},
            {doorWidth/2, roomHeight/2+cliffHeight/2-doorHeight/2, wallWidth/2},
            0, 0.1, 1.0, 10, 2, 3);
    makeObject({0, 0, 0, 1},
            { roomLength/2-0.5f+doorWidth/4,  roomHeight/2+cliffHeight/2,  roomWidth/2+wallWidth/2},
            {0.5f-doorWidth/4, roomHeight/2+cliffHeight/2, wallWidth/2},
            0, 0.1, 1.0, 11, 2, 3);
    makeObject({0, 0, 0, 1},
            {-roomLength/2+0.5f-doorWidth/4,  roomHeight/2+cliffHeight/2,  roomWidth/2+wallWidth/2},
            {0.5f-doorWidth/4, roomHeight/2+cliffHeight/2, wallWidth/2},
            0, 0.1, 1.0, 11, 2, 3);
    
    makeObject({0, 0, 0, 1}, {                        0,  roomHeight/2+cliffHeight/2, -roomWidth/2-wallWidth/2}, {roomLength/2, roomHeight/2+cliffHeight/2, wallWidth  }, 0, 0.1, 1.0, 3, 2, 3);
    makeObject({0, 0, 0, 1}, { roomLength/2+wallWidth/2,  roomHeight/2+cliffHeight/2,                        0}, {wallWidth   , roomHeight/2+cliffHeight/2, roomWidth/2}, 0, 0.1, 1.0, 4, 2, 3);
    makeObject({0, 0, 0, 1}, {-roomLength/2-wallWidth/2,  roomHeight/2+cliffHeight/2,                        0}, {wallWidth   , roomHeight/2+cliffHeight/2, roomWidth/2}, 0, 0.1, 1.0, 4, 2, 3);
    
    //Rim
    makeObject({0, 0, 0, 1}, {                        0,  rimHeight2,  -roomWidth/2+wallWidth/2}, {roomLength/2, rimHeight1/2, wallWidth/4}, 0, 0.1, 1.0, 5, 2, 0);
    makeObject({0, 0, 0, 1}, { roomLength/2-wallWidth/2,  rimHeight2,                         1}, {wallWidth/4 , rimHeight1/2, roomWidth/2-1}, 0, 0.1, 1.0, 6, 2, 0);
    makeObject({0, 0, 0, 1}, {-roomLength/2+wallWidth/2,  rimHeight2,                         0}, {wallWidth/4 , rimHeight1/2, roomWidth/2}, 0, 0.1, 1.0, 6, 2, 0);
    
    indicator = makeObject({0, 0, 0}, {1, 0, 0, 0},  {1, 1, 1}, 0, 0);
    
    /*
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
    */
    
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
    float roomWidth = 8.0f, roomLength = 12.f, roomHeight = 2.7f, wallWidth = 0.1f, rimHeight1 = 0.2f, rimHeight2 = 0.9f, cliffWidth = 3.5f, cliffHeight = 0.8f;
    float doorWidth = 1.2f, doorHeight = 2.2f, doorOffset = 1.6f;
    
    btTransform trans;
    user->rigidBody->getMotionState()->getWorldTransform(trans);
    
    if(trans.getOrigin().getZ() > roomWidth/2) {
        trans = btTransform{{0, 0, 0, 1}, {-5, 1, 3}};
        camTheta = -PI;
        camPhi = 0;
        user->rigidBody->setWorldTransform(trans);
        user->rigidBody->setLinearVelocity({0, 0, 0});
    }
    
    bool onGround = trans.getOrigin().getY() < 0.9;
    
    glm::vec3 camPos = glm::vec3{   trans.getOrigin().getX()-0.2f*peekingAnimation*cos(camTheta),
                                    trans.getOrigin().getY()+0.4f-0.8f*crouchingAnimation,
                                    trans.getOrigin().getZ()+0.2f*peekingAnimation*sin(camTheta)};
    glm::vec3 camDir = glm::vec3{cos(camPhi)*sin(camTheta), sin(camPhi), cos(camPhi)*cos(camTheta)};
    
    float indicatorDist = 0;
    glm::vec3 indicatorPos = camPos;
    
    Interactable * interactable = interacter->lookingAt(camPos, camDir);
    if(interactable) {
        indicatorPos = interactable->indicatorPos;
        indicatorDist = -1;          //Just needs to be non-zero; also used as a signal that this is not on the whiteboard
        indicatorColor = interactable->isEnabled ? glm::vec4{0, 1, 0, 0.5f} : glm::vec4{1, 0, 0, 0.5f};
        if(interactionStage == 1) {interactable->function(); interactionStage = 2;}
    } else {
        std::pair<float, glm::vec3> inter = whiteboard->intersect(camPos, camDir);
        if(inter.first > 0) {
            indicatorDist = inter.first;
            indicatorPos = inter.second;
            indicatorColor = paintColor;
            indicatorColor.a = 0.5f;
        }
    }
    
    indicator->getModel() = glm::translate(indicatorPos) *
                            glm::scale(glm::vec3{1, 1, 1}*(indicatorDist > 0 ? paintRadius*2 : (indicatorDist == -1 ? 0.1f : 0)));
                            
    
    if(indicatorDist != 0 && paint) {
        glm::vec3 l = (glm::vec3)(whiteboard->getModel()*glm::vec4{-1, -1, -1,  1});
        glm::vec3 r = (glm::vec3)(whiteboard->getModel()*glm::vec4{ 1,  1,  1,  1});
        glm::vec2 uv{1 - (indicatorPos.x-l.x)/(r.x-l.x), 1 - (indicatorPos.y-l.y)/(r.y-l.y)};
        if(hasPrev) {
            ih->paint(paintColor, uvPrev, uv, paintRadius, &whiteboardImage, false);
            ih->loadTexture(whiteboardTexture, &whiteboardImage);
        } else {hasPrev = true;}
        uvPrev = uv;
    } else {
        hasPrev = false;
    }
    
    if(doorLeftC) {
        doorLeftA += dt/1;
        if(doorLeftA > PI/3) {doorLeftA = PI/3;}
    } else {
        doorLeftA -= dt/1;
        if(doorLeftA < 0) {doorLeftA = 0;}
    }
    doorLeftI->indicatorPos = glm::vec3{
            roomLength/2-1+doorWidth/2+(0.2f-doorWidth)*cos(doorLeftA)-0.1f*sin(doorLeftA),
            doorHeight/2,
            roomWidth/2+wallWidth/2-(0.2f-doorWidth)*sin(doorLeftA)-0.1f*cos(doorLeftA)};
    
    btTransform doorLT{
            {0, sin(PI/4+doorLeftA/2), 0, cos(PI/4+doorLeftA/2)},
            { roomLength/2-1+doorWidth/2-doorWidth/2*cos(doorLeftA), doorHeight/2, roomWidth/2+wallWidth/2+doorWidth/2*sin(doorLeftA)}};
    doorLeft->rigidBody->getMotionState()->setWorldTransform(doorLT);
    
    if(doorRightC) {
        doorRightA += dt/1;
        if(doorRightA > PI/3) {doorRightA = PI/3;}
    } else {
        doorRightA -= dt/1;
        if(doorRightA < 0) {doorRightA = 0;}
    }
    doorRightI->indicatorPos = glm::vec3{
            -roomLength/2+1+doorWidth/2+(0.2f-doorWidth)*cos(doorRightA)-0.1f*sin(doorRightA),
            doorHeight/2,
            roomWidth/2+wallWidth/2-(0.2f-doorWidth)*sin(doorRightA)-0.1f*cos(doorRightA)};
    
    btTransform doorRT{
            {0, sin(PI/4+doorRightA/2), 0, cos(PI/4+doorRightA/2)},
            {-roomLength/2+1+doorWidth/2-doorWidth/2*cos(doorRightA), doorHeight/2, roomWidth/2+wallWidth/2+doorWidth/2*sin(doorRightA)}};
    doorRight->rigidBody->getMotionState()->setWorldTransform(doorRT);
    
    ball->rigidBody->getMotionState()->getWorldTransform(trans);
    if(trans.getOrigin().getY() < 0 && !throwBall) {hasBall = true;}
    if(hasBall) {
        if(throwBall) {
            trans = btTransform{{0, 0, 0, 1}, {camPos.x+camDir.x, camPos.y+camDir.y, camPos.z+camDir.z}};
            ball->rigidBody->setWorldTransform(trans);
            ball->rigidBody->setLinearVelocity({camDir.x*10, camDir.y*10, camDir.z*10});
            ball->rigidBody->setAngularVelocity({0, 0, 0});
            hasBall = false;
        } else {
            trans = btTransform{{0, 0, 0, 1}, {0, -2, 0}};
            ball->rigidBody->setWorldTransform(trans);
        }
    }
    ball->rigidBody->getMotionState()->getWorldTransform(trans);
    ballI->position = glm::vec3{trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()};
    ballI->indicatorPos = ballI->position + glm::vec3{0, 0.3f, 0};
    //std::cout << "lol: " << trans.getOrigin().getX() << ", " << trans.getOrigin().getY() << ", " << trans.getOrigin().getZ() << std::endl;
    
    
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
    
    const btVector3 & v = user->rigidBody->getLinearVelocity();
    if(onGround) {
        user->rigidBody->setLinearVelocity({v.getX()*(1-4*dt), v.getY(), v.getZ()*(1-4*dt)});
    }
    
    user->rigidBody->applyCentralForce({
            (sin(camTheta)*dz+cos(camTheta)*dx)*(onGround ? 1 : 0.3f)*walkingSpeed*(dy > 0 ? 2 : 1),
            dy*onGround*(walkingType == -1 ? 0 : 1),
            (cos(camTheta)*dz-sin(camTheta)*dx)*(onGround ? 1 : 0.3f)*walkingSpeed*(dy > 0 ? 2 : 1)});
    
    dynamicsWorld->stepSimulation(dt, 10);
    for(Object * object : objects) {
        object->update(dt);
    }
}

void Graphics::Render() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    btTransform trans;
    user->rigidBody->getMotionState()->getWorldTransform(trans);
    glm::vec3 camPos{trans.getOrigin().getX()-0.2f*peekingAnimation*cos(camTheta), trans.getOrigin().getY()+0.4f-0.8f*crouchingAnimation, trans.getOrigin().getZ()+0.2f*peekingAnimation*sin(camTheta)};
    glm::vec3 camDir{sin(camTheta)*cos(camPhi), sin(camPhi), cos(camTheta)*cos(camPhi)};
    m_camera->SetView(camPos, camPos + camDir);
    
    static Shader * shader;
    if(lighting == 3) {shader = shaderFL;}
    else if(lighting == 4) {shader = shaderVL;}
    shader->Enable();
    if(lighting >= 3) {lighting -= 2;}
    
    glUniformMatrix4fv(shader->uniform("projMat"), 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection())); 
    glUniformMatrix4fv(shader->uniform("viewMat"), 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
    glUniform1i(shader->uniform("tex"), 0);
    glUniform3fv(shader->uniform("camPos"), 1, glm::value_ptr(m_camera->GetPosition()));
    glUniform3fv(shader->uniform("lightBrightness"), 1, glm::value_ptr(lightsOn ? roomBrightness : glm::vec3{0, 0, 0}));
    
    
    for(Object * object : objects) {
        glUniform3fv(shader->uniform("flashPos"), 1, glm::value_ptr(camPos-camDir));
        glUniform3fv(shader->uniform("flashDir"), 1, glm::value_ptr(camDir));
        glUniform3fv(shader->uniform("flashBrightness"), 1, glm::value_ptr(flashOn == 1 ? glm::vec3{2.6f, 2.5f, 2.4f} : glm::vec3{0, 0, 0}));
        glUniform1i(shader->uniform("override"), object == indicator ? 1 : 0);
        glUniform4fv(shader->uniform("overrideColor"), 1, glm::value_ptr(indicatorColor));
        glUniformMatrix4fv(shader->uniform("modelMat"), 1, GL_FALSE, glm::value_ptr(object->getModel()));
        glUniformMatrix4fv(shader->uniform("normalMat"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(object->getModel()))));
        glUniform3fv(shader->uniform("ka"), 1, glm::value_ptr(object->getKa()));
        glUniform3fv(shader->uniform("kd"), 1, glm::value_ptr(object->getKd()));
        glUniform3fv(shader->uniform("ks"), 1, glm::value_ptr(object->getKs()));
        glUniform1f(shader->uniform("shininess"), object->getShininess());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, object->getTexture());
        object->render();
    }
    
    menu->render();
    
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

void loadMesh(std::string filename, std::vector<VertexData> & vertices, std::vector<unsigned int> & indices, btTriangleMesh * trimesh) {
    Assimp::Importer importer;
    const aiScene * scene = importer.ReadFile("objects/models/" + filename, aiProcess_Triangulate);
    if(scene == 0) {std::cerr << "No file " << filename << " exists!" << std::endl;}
    
    aiVector3D zero3D(0.f, 0.f, 0.f);
    
    int offset = 0;
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
            indices.push_back(offset+face.mIndices[0]);
            indices.push_back(offset+face.mIndices[1]);
            indices.push_back(offset+face.mIndices[2]);
            if(trimesh) {
                const glm::vec3 & v0 = vertices[offset+face.mIndices[0]].position, & v1 = vertices[offset+face.mIndices[1]].position, & v2 = vertices[offset+face.mIndices[2]].position;
                trimesh->addTriangle({v0.x, v0.y, v0.z}, {v1.x, v1.y, v1.z}, {v2.x, v2.y, v2.z});
            }
        }
        offset += mesh->mNumVertices;
    }
}

