#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "SDL_all.h"
#include "SDLFps.hpp"
#include "SDLTimer.hpp"
#include "Window.h"
#include "TaskCard.hpp"

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
    bool isSmall = false;
    SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
    TaskCard taskCard;
    Task taskTest;


    int menuLine = int(SCREEN_WIDTH*0.7) -SCREEN_WIDTH/2; 
    //フルサイズの領域
    SDL_FRect TaskRect={SCREEN_WIDTH/4,SCREEN_HEIGHT/3,int(SCREEN_WIDTH*0.7),int(SCREEN_HEIGHT*0.6)};
    SDL_FRect MenuRect={0,0,float(menuLine),SCREEN_HEIGHT};
    SDL_FRect MenuBox ={0,0,float(menuLine),SCREEN_HEIGHT/10};
    //Smallサイズの領域
    SDL_FRect s_taskMenuLineRect = {0,SMALL_SCH/6,SMALL_SCW, int(SMALL_SCH/6*5)};
    SDL_FRect s_taskMenuRect = {10,SMALL_SCH/6+10,SMALL_SCW-20,int(SMALL_SCH/6*5-20)};
    SDL_FRect s_task_A = {10,SMALL_SCH/6+10,SMALL_SCW-20,int((SMALL_SCH/6*5-20)/10)};
    SDL_FRect s_charRect = {10,10,SMALL_SCH/6,SMALL_SCH/6-10};
    SDL_Rect JudgeCharRect = {10,10,SMALL_SCH/6,SMALL_SCH/6-10};
    SDL_FRect s_fukidashiRect = {SMALL_SCW/3 + 20, 40, SMALL_SCW/3*2, SMALL_SCH/6-20};
    
    Uint32 uFlames = 0;
    const bool *pKeyStatus;
    bool bQuit = false;
    bool most_front = false;
    float fFPS;
      


    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    window = SDL_CreateWindow("roommate", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_TRANSPARENT);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderDrawBlendMode(renderer, blendMode);

    //テクスチャの作成
    charSuf = IMG_Load("assets/minikyaraTest.png");
    minicharTex = SDL_CreateTextureFromSurface(renderer,charSuf);
    SDL_DestroySurface(charSuf);

    
    //フォントのロード
    font = TTF_OpenFont("assets/BestTen-DOT.otf",FONTSIZE);
    fsuf = TTF_RenderText_Blended(font,"メニュー",0,fColor);
    ftex = SDL_CreateTextureFromSurface(renderer,fsuf);
    SDL_DestroySurface(fsuf);
    
    int id = 1;
    int ATflags = false;
    taskTest = taskCard.ReadTaskDB(id,ATflags);
    std::string NewTaskContent = "gatiiku";
    taskCard.newWriteTaskDB(NewTaskContent);
    taskCard.flagChange(id);
    
    //ゲームループ
    while (bQuit == false)
   {
        timeModifyFPS.restart();
        SDL_PumpEvents();
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT){
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
                isSmall = true;
            }else{
                most_front = false;
                isSmall = false;
            }
        }
        SetWindow(window,isSmall);
        if(isSmall){
            SDL_SetWindowHitTest(window, SmallWindow_Drag, &JudgeCharRect);
        }


        //描画処理
        //フルサイズ画面のホーム描画処理
        if (isSmall){
            SDL_SetRenderDrawColor(renderer,0,0,0,0);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer,110,105,148,255);//線の色
            SDL_RenderFillRect(renderer,&s_taskMenuLineRect);
            SDL_SetRenderDrawColor(renderer,161,157,185,255);
            SDL_RenderFillRect(renderer,&s_taskMenuRect);        
            SDL_FRect s_charDrawRect = {float(s_charRect.x), float(s_charRect.y), float(s_charRect.w), float(s_charRect.h)};
            SDL_RenderTexture(renderer,minicharTex,NULL, &s_charDrawRect);
            taskCard.DrawTaskCard(renderer,taskTest,s_task_A,font);
        }
        else{
            SDL_SetRenderDrawColor(renderer,186,183,204,255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer,161,157,185,255);
            SDL_RenderFillRect(renderer,&MenuRect);        
            SDL_SetRenderDrawColor(renderer,110,105,148,255);//線の色
            SDL_RenderLine(renderer,menuLine,0,menuLine,SCREEN_HEIGHT);
            SDL_RenderLine(renderer,0,SCREEN_HEIGHT/10,menuLine,SCREEN_HEIGHT/10);
            SDL_RenderLine(renderer,0,SCREEN_HEIGHT/5,menuLine,SCREEN_HEIGHT/5);
            SDL_RenderLine(renderer,0,SCREEN_HEIGHT/10*3,menuLine,SCREEN_HEIGHT/10*3);
            SDL_RenderRect(renderer,&TaskRect);
            SDL_RenderTexture(renderer,ftex,NULL, &MenuBox);
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

    SDL_Quit();

    return 0;
}
