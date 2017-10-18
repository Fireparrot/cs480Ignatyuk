#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <string>

//The only change to the window class is mouse grabbing/hiding
class Window {
public:
    Window();
    ~Window();
    bool Initialize(const std::string &name, int* width, int* height);
    void Swap();

    SDL_Window* gWindow;
private:
    SDL_GLContext gContext;
};

#endif /* WINDOW_H */
