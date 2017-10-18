
#include "engine.h"

Engine::Engine(string name, int width, int height) {
    m_WINDOW_NAME = name;
    m_WINDOW_WIDTH = width;
    m_WINDOW_HEIGHT = height;
    m_FULLSCREEN = false;
}

Engine::Engine(string name) {
    m_WINDOW_NAME = name;
    m_WINDOW_HEIGHT = 0;
    m_WINDOW_WIDTH = 0;
    m_FULLSCREEN = true;
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
    
    if (m_event.type == SDL_KEYDOWN) {
        if(m_event.key.keysym.sym == SDLK_ESCAPE) {
          m_running = false;
        }
        if(m_event.key.keysym.sym == SDLK_RIGHT) {
          m_graphics->camTheta +=  1.f*m_DT/1000;
        }
        if(m_event.key.keysym.sym == SDLK_LEFT) {
          m_graphics->camTheta += -1.f*m_DT/1000;
        }
        if(m_event.key.keysym.sym == SDLK_UP) {
          m_graphics->camPhi +=  1.f*m_DT/1000;
        }
        if(m_event.key.keysym.sym == SDLK_DOWN) {
          m_graphics->camPhi += -1.f*m_DT/1000;
        }
        if(m_graphics->camPhi >  M_PI/2 - 0.01f) {m_graphics->camPhi =  M_PI/2 - 0.01f;}
        if(m_graphics->camPhi < -M_PI/2 + 0.01f) {m_graphics->camPhi = -M_PI/2 + 0.01f;}
        if(m_event.key.keysym.sym == SDLK_l) {
            m_graphics->planetFocus += 1;
            if(m_graphics->planetFocus == 10) {m_graphics->planetFocus = 1;}
        }
        if(m_event.key.keysym.sym == SDLK_k) {
            m_graphics->planetFocus -= 1;
            if(m_graphics->planetFocus == 0) {m_graphics->planetFocus = 9;}
        }
        if(m_event.key.keysym.sym == SDLK_KP_PLUS) {
            m_graphics->timeMultiplier *= 1.1f;
        }
        if(m_event.key.keysym.sym == SDLK_KP_MINUS) {
            m_graphics->timeMultiplier /= 1.1f;
            if(m_graphics->timeMultiplier < 0.01f) {m_graphics->timeMultiplier = 0.01f;}
        }
        if(m_event.key.keysym.sym == SDLK_KP_MULTIPLY) {
            m_graphics->distanceDivisor /= 1.1f;
            if(m_graphics->distanceDivisor < 1.f) {m_graphics->distanceDivisor = 1.f;}
        }
        if(m_event.key.keysym.sym == SDLK_KP_DIVIDE) {
            m_graphics->distanceDivisor *= 1.1f;
        }
    }
}
void Engine::Mouse() {
    if(m_event.type == SDL_QUIT) {
        m_running = false;
        return;
    }
    if(m_event.type == SDL_MOUSEMOTION) {
        m_graphics->camTheta += m_event.motion.xrel/40.f * m_DT/1000;
        m_graphics->camPhi   += m_event.motion.yrel/40.f * m_DT/1000;
        if(m_graphics->camPhi >  M_PI/2 - 0.01f) {m_graphics->camPhi =  M_PI/2 - 0.01f;}
        if(m_graphics->camPhi < -M_PI/2 + 0.01f) {m_graphics->camPhi = -M_PI/2 + 0.01f;}
        //std::cout << m_event.motion.xrel << ", " << m_event.motion.yrel << std::endl;
        int x, y;
        SDL_GetRelativeMouseState(&x, &y);
        //std::cout << "  " << m_event.motion.x << ", " << m_event.motion.y << "  |  " << x << ", " << y << std::endl;
        //std::cout << "    " << m_graphics->camTheta << " / " << m_graphics->camPhi << std::endl;
    }
    if(m_event.type == SDL_MOUSEWHEEL) {
        if(m_event.wheel.y ==  1) {m_graphics->zoom /= 1.1f;}
        if(m_event.wheel.y == -1) {m_graphics->zoom *= 1.1f;}
        if(m_graphics->zoom < 0.2f) {m_graphics->zoom = 0.2f;}
    }
    //SDL_WarpMouseGlobal(m_WINDOW_WIDTH/2, m_WINDOW_HEIGHT/2);
    /*
    if(m_event.motion.x > m_WINDOW_WIDTH-2) {
        SDL_WarpMouseInWindow(m_window->gWindow, m_WINDOW_WIDTH-2, m_event.motion.y);
    }
    if(m_event.motion.x < 1) {
        SDL_WarpMouseInWindow(m_window->gWindow, 1, m_event.motion.y);
    }
    if(m_event.motion.y > m_WINDOW_HEIGHT-2) {
        SDL_WarpMouseInWindow(m_window->gWindow, m_event.motion.x, m_WINDOW_HEIGHT-2);
    }
    if(m_event.motion.y < 1) {
        SDL_WarpMouseInWindow(m_window->gWindow, m_event.motion.x, 1);
    }
    */
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

