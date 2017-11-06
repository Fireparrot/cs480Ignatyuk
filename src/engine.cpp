
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
    
    cylX = cylZ = 0;
    
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
            m_graphics->dx += cylX*float(m_DT)*5/1000.f;
            m_graphics->dz += cylZ*float(m_DT)*5/1000.f;
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
    
    if(m_event.type == SDL_KEYDOWN) {
        if(m_event.key.keysym.sym == SDLK_ESCAPE) {
          m_running = false;
        }
        if(m_event.key.keysym.sym == SDLK_w) {
            cylZ = 1;
        }
        if(m_event.key.keysym.sym == SDLK_s) {
            cylZ = -1;
        }
        if(m_event.key.keysym.sym == SDLK_a) {
            cylX = 1;
        }
        if(m_event.key.keysym.sym == SDLK_d) {
            cylX = -1;
        }
        if(m_event.key.keysym.sym == SDLK_e) {
            if(m_graphics->lighting == 1) {m_graphics->lighting = 4;}
            else if(m_graphics->lighting == 2) {m_graphics->lighting = 3;}
        }
        if(m_event.key.keysym.sym == SDLK_KP_MULTIPLY) {
            m_graphics->brightness += 80*m_DT/1000.f;
        }
        if(m_event.key.keysym.sym == SDLK_KP_DIVIDE) {
            m_graphics->brightness -= 80*m_DT/1000.f;
        }
        if(m_event.key.keysym.sym == SDLK_KP_PLUS) {
            float & full = m_graphics->full;
            float da = sqrt(1 - full*full);
            full -= da*m_DT/1000.f;
            if(full < -0.98f) {full = -0.98f;}
        }
        if(m_event.key.keysym.sym == SDLK_KP_MINUS) {
            float & full = m_graphics->full;
            float da = sqrt(1 - full*full);
            full += da*m_DT/1000.f;
            if(full > 0.98f) {full = 0.98f;}
        }
        if(m_event.key.keysym.sym == SDLK_r) {
            m_graphics->increaseAL( 0.2f*m_DT/1000.f);
        }
        if(m_event.key.keysym.sym == SDLK_f) {
            m_graphics->increaseAL(-0.2f*m_DT/1000.f);
        }
        if(m_event.key.keysym.sym == SDLK_t) {
            m_graphics->increaseSL( 0.2f*m_DT/1000.f);
        }
        if(m_event.key.keysym.sym == SDLK_g) {
            m_graphics->increaseSL(-0.2f*m_DT/1000.f);
        }
        if(m_event.key.keysym.sym == SDLK_c) {
            m_graphics->cyanLight = !m_graphics->cyanLight;
        }
    }
    if(m_event.type == SDL_KEYUP) {
        if(m_event.key.keysym.sym == SDLK_w) {
            cylZ = 0;
        }
        if(m_event.key.keysym.sym == SDLK_s) {
            cylZ = 0;
        }
        if(m_event.key.keysym.sym == SDLK_a) {
            cylX = 0;
        }
        if(m_event.key.keysym.sym == SDLK_d) {
            cylX = 0;
        }
    }
}
void Engine::Mouse() {
    if(m_event.type == SDL_QUIT) {
        m_running = false;
        return;
    }
    if(m_event.type == SDL_MOUSEMOTION) {
        m_graphics->dx -= m_event.motion.xrel/1.f * m_DT/1000;
        m_graphics->dz -= m_event.motion.yrel/1.f * m_DT/1000;
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

