#include "SDL_all.h"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main(int argc, char* argv[]){
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;
    const Uint8 *pKeyStatus;
    bool bQuit = false;


    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    window = SDL_CreateWindow("Simple Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN+SDL_WINDOW_BORDERLESS+SDL_WINDOW_ALWAYS_ON_TOP);
    renderer = SDL_CreateRenderer(window, -1, 0);

    
    //ゲームループ
    while (bQuit == false)
   {
        SDL_PumpEvents();
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT){
                bQuit = true;
            }
        }
        pKeyStatus = SDL_GetKeyboardState(nullptr);
        if (pKeyStatus[SDL_SCANCODE_ESCAPE]){
            bQuit = true;
        }

    }


    
    
    
    
    
    
    
    
    
    
    
    
    //破棄
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
