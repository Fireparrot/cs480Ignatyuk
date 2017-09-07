#include <window.h>

Window::Window()
{
  gWindow = NULL;
}

Window::~Window()
{
  SDL_StopTextInput();
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  SDL_Quit();
}

bool Window::Initialize(const string &name, int* width, int* height)
{
    // Start SDL
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL failed to initialize: %s\n", SDL_GetError());
    return false;
  }

  // Start OpenGL for SDL
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );

  // Create window
  SDL_DisplayMode current;
  SDL_GetCurrentDisplayMode(0, &current);
  
  //use for fullscreen
  if (*height == 0 && *width == 0)
  {
    *height = current.h;
    *width = current.w;
  }

  gWindow = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, *width, *height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
  if(gWindow == NULL)
  {
    printf("Widow failed to create: %s\n", SDL_GetError());
    return false;
  }

  // Create context
  gContext = SDL_GL_CreateContext(gWindow);
  if(gContext == NULL)
  {
    printf("OpenGL context not created: %s\n", SDL_GetError());
    return false;
  }

  // Use VSync
  if(SDL_GL_SetSwapInterval(1) < 0)
  {
    printf("Unable to use VSync: %s\n", SDL_GetError());
    return false;
  }

  return true;
}

void Window::Swap()
{
  SDL_GL_SwapWindow(gWindow);
}
