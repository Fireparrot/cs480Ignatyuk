#include <iostream>

#include "engine.h"


int main(int argc, char **argv) {
    //Start an engine and run it, cleanup afterwards
    Engine *engine = new Engine("Project 3", 1920, 1080);
    if(!engine->Initialize()) {
        printf("The engine failed to start.\n");
        delete engine;
        engine = NULL;
        return 1;
    }
    engine->Run();
    delete engine;
    engine = NULL;
    return 0;
}
