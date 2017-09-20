#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <string>

class Window {
public:
    Window();
    ~Window();
    bool Initialize(const std::string &name, int* width, int* height);
    void Swap();

private:
    SDL_Window* gWindow;
    SDL_GLContext gContext;
};

#endif /* WINDOW_H */
