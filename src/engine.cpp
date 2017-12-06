
#include "engine.h"

#define PI 3.141592653589793238

Engine::Engine(string name, int width, int height) {
    m_WINDOW_NAME = name;
    m_WINDOW_WIDTH = width;
    m_WINDOW_HEIGHT = height;
    m_FULLSCREEN = false;
    mouseWarped = false;
}

Engine::Engine(string name) {
    m_WINDOW_NAME = name;
    m_WINDOW_HEIGHT = 0;
    m_WINDOW_WIDTH = 0;
    m_FULLSCREEN = true;
    mouseWarped = false;
}

Engine::~Engine() {
    delete m_window;
    delete m_graphics;
    m_window = NULL;
    m_graphics = NULL;
}

bool Engine::Initialize() {
    //Start a window
    m_window = new Window();
    if(!m_window->Initialize(m_WINDOW_NAME, &m_WINDOW_WIDTH, &m_WINDOW_HEIGHT)) {
        printf("The window failed to initialize.\n");
        return false;
    }
    
    //Start the graphics
    m_graphics = new Graphics();
    if(!m_graphics->Initialize(m_WINDOW_WIDTH, m_WINDOW_HEIGHT)) {
        printf("The graphics failed to initialize.\n");
        return false;
    }
    
    //Set the time
    m_currentTimeMillis = GetCurrentTimeMillis();
    
    //No errors
    return true;
}

void Engine::Run() {
    m_running = true;
    
    while(m_running) {
        //Update the DT
        m_DT = getDT();
        
        //Check the keyboard input
        while(SDL_PollEvent(&m_event) != 0) {
            Keyboard();
            Mouse();
        }
        SDL_WarpMouseInWindow(NULL, m_WINDOW_WIDTH/2, m_WINDOW_HEIGHT/2);
        mouseWarped = true;
        
        //Update and render the graphics
        m_graphics->Update(m_DT/1000.f);
        m_graphics->Render();
        
        //Swap to the Window
        m_window->Swap();
    }
}

void Engine::Keyboard() {
    if(m_event.type == SDL_QUIT) {
        m_running = false;
        return;
    }
    
    if(m_event.type == SDL_KEYDOWN) {
        //std::cerr << "DOWN: " << m_event.key.keysym.sym << std::endl;
        if(m_event.key.keysym.sym == SDLK_ESCAPE) {
          m_running = false;
        }
        
        if(m_event.key.keysym.sym == SDLK_l) {
            if(m_graphics->lighting < 3) {m_graphics->lighting = 5 - m_graphics->lighting;}
        }
        if(m_event.key.keysym.sym == SDLK_w) {
            if(m_graphics->dz == 0) {
                m_graphics->dz = 500;
            }
        }
        if(m_event.key.keysym.sym == SDLK_a) {
            if(m_graphics->dx == 0) {
                m_graphics->dx = 500;
            }
        }
        if(m_event.key.keysym.sym == SDLK_s) {
            if(m_graphics->dz == 0) {
                m_graphics->dz = -500;
            }
        }
        if(m_event.key.keysym.sym == SDLK_d) {
            if(m_graphics->dx == 0) {
                m_graphics->dx = -500;
            }
        }
        if(m_event.key.keysym.sym == SDLK_SPACE) {
            m_graphics->dy = 6000;
        }
        if(m_event.key.keysym.sym == SDLK_LSHIFT) {
            if(m_graphics->walkingType == 0) {
                m_graphics->walkingType = 1;
            }
        }
        if(m_event.key.keysym.sym == SDLK_LCTRL) {
            if(m_graphics->walkingType == 0) {
                m_graphics->walkingType = -1;
            }
        }
        if(m_event.key.keysym.sym == SDLK_q) {
            if(m_graphics->peek == 0) {
                m_graphics->peek = -1;
            }
        }
        if(m_event.key.keysym.sym == SDLK_e) {
            if(m_graphics->peek == 0) {
                m_graphics->peek = 1;
            }
        }
    }
    if(m_event.type == SDL_KEYUP) {
        if(m_event.key.keysym.sym == SDLK_w) {
            if(m_graphics->dz > 0) {
                m_graphics->dz = 0;
            }
        }
        if(m_event.key.keysym.sym == SDLK_a) {
            if(m_graphics->dx > 0) {
                m_graphics->dx = 0;
            }
        }
        if(m_event.key.keysym.sym == SDLK_s) {
            if(m_graphics->dz < 0) {
                m_graphics->dz = 0;
            }
        }
        if(m_event.key.keysym.sym == SDLK_d) {
            if(m_graphics->dx < 0) {
                m_graphics->dx = 0;
            }
        }
        if(m_event.key.keysym.sym == SDLK_SPACE) {
            m_graphics->dy = 0;
        }
        if(m_event.key.keysym.sym == SDLK_LSHIFT) {
            if(m_graphics->walkingType == 1) {
                m_graphics->walkingType = 0;
            }
        }
        if(m_event.key.keysym.sym == SDLK_LCTRL) {
            if(m_graphics->walkingType == -1) {
                m_graphics->walkingType = 0;
            }
        }
        if(m_event.key.keysym.sym == SDLK_q) {
            if(m_graphics->peek < 0) {
                m_graphics->peek = 0;
            }
        }
        if(m_event.key.keysym.sym == SDLK_e) {
            if(m_graphics->peek > 0) {
                m_graphics->peek = 0;
            }
        }
    }
}
void Engine::Mouse() {
    if(m_event.type == SDL_QUIT) {
        m_running = false;
        return;
    }
    if(m_event.type == SDL_MOUSEMOTION) {
        if(mouseWarped) {mouseWarped = false;}
        else {
            m_graphics->camTheta -= m_event.motion.xrel/10.f * m_DT/1000;
            m_graphics->camPhi -= m_event.motion.yrel/10.f * m_DT/1000;
            if(m_graphics->camPhi < -PI/2 * 0.98) {m_graphics->camPhi = -PI/2 * 0.98;}
            if(m_graphics->camPhi >  PI/2 * 0.98) {m_graphics->camPhi =  PI/2 * 0.98;}
        }
    }
}

unsigned int Engine::getDT() {
    long long TimeNowMillis = GetCurrentTimeMillis();
    assert(TimeNowMillis >= m_currentTimeMillis);
    unsigned int DeltaTimeMillis = (unsigned int)(TimeNowMillis - m_currentTimeMillis);
    m_currentTimeMillis = TimeNowMillis;
    return DeltaTimeMillis;
}

long long Engine::GetCurrentTimeMillis() {
    timeval t;
    gettimeofday(&t, NULL);
    long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
    return ret;
}

