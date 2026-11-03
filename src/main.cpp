#include "SDL_all.h"
#include "SDLFps.hpp"
#include "SDLTimer.hpp"

#define SCREEN_WIDTH 640*2
#define SCREEN_HEIGHT 480*2
#define FONTSIZE 32 

int main(int argc, char* argv[]){
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;
    SDLTimer timerCountFPS;
    SDLTimer timeModifyFPS;
    SDLFps timeModify; 
    TTF_Font *font;
    SDL_Surface *fsuf = nullptr;
    SDL_Texture *ftex = nullptr;
    SDL_Color fColor = {0,0,0,255};


    int menuLine = int(SCREEN_WIDTH*0.7) -SCREEN_WIDTH/2; 
    SDL_Rect TaskRect={SCREEN_WIDTH/4,SCREEN_HEIGHT/3,int(SCREEN_WIDTH*0.7),int(SCREEN_HEIGHT*0.6)};
    SDL_Rect MenuRect={0,0,menuLine,SCREEN_HEIGHT};
    SDL_Rect MenuBox ={0,0,menuLine,SCREEN_HEIGHT/10};
    
    Uint32 uFlames;
    const Uint8 *pKeyStatus;
    bool bQuit = false;
    bool most_front = false;
    int num_window = 0;
    float fFPS;
    


    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    window = SDL_CreateWindow("Simple Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN+SDL_WINDOW_BORDERLESS+SDL_WINDOW_ALWAYS_ON_TOP);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer,186,183,204,255);
    SDL_RenderClear(renderer);
    num_window++;
    
    //フォントのロード
    font = TTF_OpenFont("assets/BestTen-DOT.otf",FONTSIZE);
    fsuf = TTF_RenderUTF8_Blended(font,"メニュー",fColor);
    ftex = SDL_CreateTextureFromSurface(renderer,fsuf);
    SDL_FreeSurface(fsuf);
    
    //ゲームループ
    while (bQuit == false)
   {
        timeModifyFPS.restart();
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
        if (pKeyStatus[SDL_SCANCODE_Q]){
            most_front = true;
        }

        

        if (most_front && num_window <2){
            window = SDL_CreateWindow("Simple Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
            num_window++;
            most_front = false;
        }

        //描画処理
        //フルサイズ画面のホーム描画処理
        SDL_SetRenderDrawColor(renderer,161,157,185,255);
        SDL_RenderFillRect(renderer,&MenuRect);        
        SDL_SetRenderDrawColor(renderer,110,105,148,255);//線の色
        SDL_RenderDrawLine(renderer,menuLine,0,menuLine,SCREEN_HEIGHT);
        SDL_RenderDrawLine(renderer,0,SCREEN_HEIGHT/10,menuLine,SCREEN_HEIGHT/10);
        SDL_RenderDrawLine(renderer,0,SCREEN_HEIGHT/5,menuLine,SCREEN_HEIGHT/5);
        SDL_RenderDrawLine(renderer,0,SCREEN_HEIGHT/10*3,menuLine,SCREEN_HEIGHT/10*3);
        SDL_RenderDrawRect(renderer,&TaskRect);
        SDL_RenderCopy(renderer,ftex,NULL, &MenuBox);
       //フルサイズ画面の描画はここまで


        SDL_RenderPresent(renderer);




        //フレーム固定処理
        uFlames++;
        fFPS = uFlames / (timerCountFPS.getTicks()/ 1000.);
        //ピリオドをつけるとfloatになる。なければint扱い、0で割る可能性. ticksはmsなので1000で割る.

        timeModify.fpsadjust(30, timeModifyFPS);
    }
    //破棄
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
