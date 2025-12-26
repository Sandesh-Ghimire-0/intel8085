#include<iostream>
#include<cstdlib>
#include<cstdbool>
#include<SDL2/SDL.h>

// g++ main.cpp -o main `sdl2-config --cflags --libs`
int width = 400;
int height = 400;

typedef struct {   // this will contain all main sdl creations, is that what it is called?
    SDL_Window *p_window;
    SDL_Renderer *p_renderer;
} sdl_t;

bool init_sdl(sdl_t *SDL)
{   
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "Error initing SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL->p_window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!SDL->p_window) {
        std::cout << "Error creating window: " << SDL_GetError() << std::endl;
        return false;
    }
    
    SDL->p_renderer = SDL_CreateRenderer(SDL->p_window, -1, SDL_RENDERER_ACCELERATED);
    if (!SDL->p_renderer) {
        std::cout << "Error creating renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void clean_all(sdl_t *SDL)   // our own destructor
{   
    SDL_DestroyRenderer(SDL->p_renderer);
    SDL_DestroyWindow(SDL->p_window);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    sdl_t SDL;
    (void) argc;  // so compiler doesnt complain 
    (void) argv;
    
    if (!init_sdl(&SDL)) {
        return 1;
    }
    
    bool done = false;
    
    while (!done) {     // infinite run until X 
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = true;
            }
        }
        
        SDL_SetRenderDrawColor(SDL.p_renderer, 255, 0, 0, 255);  // as the name says this will make renderer draw this color , this can be used in rect and stuff to make things more interesting
        SDL_RenderClear(SDL.p_renderer);  // apply to whole screen 
        SDL_RenderPresent(SDL.p_renderer);  // update the window
    }
    
    clean_all(&SDL);
    return 0;
}