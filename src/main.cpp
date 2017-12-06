#include <iostream>

#include "engine.h"

//This drives the program, duh
int main(int argc, char **argv) {
    std::cout << "Starting project..." << std::endl;

    Engine * engine = new Engine("Computer Graphics Simulator", 2000, 1250);
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
