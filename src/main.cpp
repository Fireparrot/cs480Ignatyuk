#include <iostream>

#include "engine.h"


int main(int argc, char **argv) {
    std::cout << "Starting project..." << std::endl;

    Engine * engine = new Engine("Project 7", 0, 0);
    if(!engine->Initialize()) {
        printf("The engine failed to start.\n");
        delete engine;
        engine = NULL;
        return 1;
    }
    engine->Run();
    delete engine;
    engine = NULL;
    
    std::cout << "Ending project..." << std::endl;
    
    return 0;
}
