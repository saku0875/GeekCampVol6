#include "SDL_all.h"
#include "SDLFps.hpp"
#include "SDLTimer.hpp"
#include "Window.h"


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
    SDL_Surface *charSuf = nullptr;
    SDL_Texture *minicharTex = nullptr;
    SDL_Texture *charTex = nullptr;
    SDL_Color fColor = {0,0,0,255};
    SDL_bool isSmall = SDL_FALSE;
    SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;


    int menuLine = int(SCREEN_WIDTH*0.7) -SCREEN_WIDTH/2; 
    //フルサイズの領域
    SDL_Rect TaskRect={SCREEN_WIDTH/4,SCREEN_HEIGHT/3,int(SCREEN_WIDTH*0.7),int(SCREEN_HEIGHT*0.6)};
    SDL_Rect MenuRect={0,0,menuLine,SCREEN_HEIGHT};
    SDL_Rect MenuBox ={0,0,menuLine,SCREEN_HEIGHT/10};
    //Smallサイズの領域
    SDL_Rect s_taskMenuLineRect = {0,SMALL_SCH/5,SMALL_SCW, int(SMALL_SCH/5*4)};
    SDL_Rect s_taskMenuRect = {10,SMALL_SCH/5+10,SMALL_SCW-20,int(SMALL_SCH/5*4-20)};
    SDL_Rect s_charRect = {10,10,SMALL_SCW/3,SMALL_SCH/5-20};
    SDL_Rect s_fukidashiRect = {SMALL_SCW/3 + 20, 40, SMALL_SCW/2, SMALL_SCH/4-20};
    
    Uint32 uFlames;
    const Uint8 *pKeyStatus;
    bool bQuit = false;
    bool most_front = false;
    int num_window = 0;
    float fFPS;
      


    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    window = SDL_CreateWindow("Simple Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawBlendMode(renderer, blendMode);

    //テクスチャの作成
    charSuf = IMG_Load("assets/minikyaraTest.png");
    minicharTex = SDL_CreateTextureFromSurface(renderer,charSuf);
    SDL_FreeSurface(charSuf);
    
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
            if (!most_front){
                most_front = true;
                isSmall = SDL_TRUE;
            }else{
                most_front = false;
                isSmall = SDL_FALSE;
            }
        }
        SetWindow(window,isSmall);
        if(isSmall){
            SDL_SetWindowHitTest(window, SmallWindow_Drag, s_charRect);
        }


        //描画処理
        //フルサイズ画面のホーム描画処理
        if (isSmall){
            SDL_SetRenderDrawColor(renderer,186,183,204,255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer,110,105,148,255);//線の色
            SDL_RenderFillRect(renderer,&s_taskMenuLineRect);
            SDL_SetRenderDrawColor(renderer,161,157,185,255);
            SDL_RenderFillRect(renderer,&s_taskMenuRect);        
            SDL_RenderCopy(renderer,minicharTex,NULL, &s_charRect);
        }
        else{
            SDL_SetRenderDrawColor(renderer,186,183,204,255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer,161,157,185,255);
            SDL_RenderFillRect(renderer,&MenuRect);        
            SDL_SetRenderDrawColor(renderer,110,105,148,255);//線の色
            SDL_RenderDrawLine(renderer,menuLine,0,menuLine,SCREEN_HEIGHT);
            SDL_RenderDrawLine(renderer,0,SCREEN_HEIGHT/10,menuLine,SCREEN_HEIGHT/10);
            SDL_RenderDrawLine(renderer,0,SCREEN_HEIGHT/5,menuLine,SCREEN_HEIGHT/5);
            SDL_RenderDrawLine(renderer,0,SCREEN_HEIGHT/10*3,menuLine,SCREEN_HEIGHT/10*3);
            SDL_RenderDrawRect(renderer,&TaskRect);
            SDL_RenderCopy(renderer,ftex,NULL, &MenuBox);
       }
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
