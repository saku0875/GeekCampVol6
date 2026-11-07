#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "SDL_all.h"
#include "SDLFps.hpp"
#include "SDLTimer.hpp"
#include "Window.h"
#include "TaskCard.hpp"
#include <vector>
#include <algorithm>

int main(int argc, char* argv[]){
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;
    SDLTimer timerCountFPS;
    SDLTimer timeModifyFPS;
    SDLFps timeModify; 
    TTF_Font *sfont;
    TTF_Font *Ffont;
    SDL_Surface *fsuf = nullptr;
    SDL_Texture *ftex = nullptr;
    SDL_Texture *notAchieveTexture = nullptr;
    SDL_Texture *achieveTexture = nullptr;
    SDL_Surface *charSuf = nullptr;
    SDL_Texture *minicharTex = nullptr;
    SDL_Texture *charTex = nullptr;
    SDL_Color fColor = {0,0,0,255};
    bool isSmall = false;
    bool showAchieved = false;
    SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
    TaskCard taskCard;
    std::vector<SDL_FRect> checkRects;
    std::vector<int> checkTaskIds;


    int menuLine = int(SCREEN_WIDTH*0.7) -SCREEN_WIDTH/2; 
    //フルサイズの領域
    SDL_FRect TaskRect={SCREEN_WIDTH/4,SCREEN_HEIGHT/3,int(SCREEN_WIDTH*0.7),int(SCREEN_HEIGHT*0.6)};
    SDL_FRect MenuRect={0,0,float(menuLine),SCREEN_HEIGHT};
    SDL_FRect MenuBox ={0,0,float(menuLine),SCREEN_HEIGHT/10};
    SDL_FRect NotAchieveButton = {TaskRect.x+TaskRect.w/8*6,TaskRect.y - 50, TaskRect.w/8,50};
    SDL_FRect AchieveButton = {TaskRect.x+TaskRect.w/8*7,TaskRect.y - 50, TaskRect.w/8,50};
    //Smallサイズの領域
    SDL_FRect s_taskMenuLineRect = {0,SMALL_SCH/6,SMALL_SCW, int(SMALL_SCH/6*5)};
    SDL_FRect s_taskMenuRect = {10,SMALL_SCH/6+10,SMALL_SCW-20,int(SMALL_SCH/6*5-20)};
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
    Ffont = TTF_OpenFont("assets/BestTen-DOT.otf",FONTSIZE);
    sfont = TTF_OpenFont("assets/BestTen-DOT.otf",SMALL_FONTSIZE);
    fsuf = TTF_RenderText_Blended(sfont,"メニュー",0,fColor);
    ftex = SDL_CreateTextureFromSurface(renderer,fsuf);
    SDL_DestroySurface(fsuf);
    fsuf = TTF_RenderText_Blended(sfont,"未達成",0,fColor);
    notAchieveTexture = SDL_CreateTextureFromSurface(renderer,fsuf);
    SDL_DestroySurface(fsuf);
    fsuf = TTF_RenderText_Blended(sfont,"達成済み",0,fColor);
    achieveTexture = SDL_CreateTextureFromSurface(renderer,fsuf);
    SDL_DestroySurface(fsuf);
    
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
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT){
                bool buttonClicked = false;
                bool previousShowAchieved = showAchieved;
                if (!isSmall){
                    if (event.button.x >= NotAchieveButton.x
                        && event.button.x < NotAchieveButton.x + NotAchieveButton.w
                        && event.button.y >= NotAchieveButton.y
                        && event.button.y < NotAchieveButton.y + NotAchieveButton.h){
                        showAchieved = false;
                        buttonClicked = true;
                    }else if (event.button.x >= AchieveButton.x
                        && event.button.x < AchieveButton.x + AchieveButton.w
                        && event.button.y >= AchieveButton.y
                        && event.button.y < AchieveButton.y + AchieveButton.h){
                        showAchieved = true;
                        buttonClicked = true;
                    }
                }
                if (showAchieved != previousShowAchieved){
                    checkRects.clear();
                    checkTaskIds.clear();
                }
                if (!buttonClicked){
                    for (int i = 0; i < (int)checkRects.size(); i++){
                        if (event.button.x >= checkRects[i].x
                            && event.button.x < checkRects[i].x + checkRects[i].w
                            && event.button.y >= checkRects[i].y
                            && event.button.y < checkRects[i].y + checkRects[i].h){
                            taskCard.flagChange(checkTaskIds[i]);
                            break;
                        }
                    }
                }
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
        std::vector<int> taskIds;
        ifstream taskFile("DB/Test.csv");
        string line;
        while (getline(taskFile,line)){
            if (line.empty()){
                continue;
            }
            istringstream row(line);
            string field;
            getline(row,field,',');
            taskIds.push_back(std::stoi(field));
        }
        taskFile.close();
        std::sort(taskIds.begin(),taskIds.end());

        std::vector<Task> tasks;
        for (int i = 0; i < (int)taskIds.size(); i++){
            int id = taskIds[i];
            if (i > 0 && id == taskIds[i - 1]){
                continue;
            }
            Task task = taskCard.ReadTaskDB(id,showAchieved);
            if (task.id == id){
                tasks.push_back(task);
            }
        }

        if (isSmall){
            SDL_SetRenderDrawColor(renderer,0,0,0,0);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer,110,105,148,255);//線の色
            SDL_RenderFillRect(renderer,&s_taskMenuLineRect);
            SDL_SetRenderDrawColor(renderer,161,157,185,255);
            SDL_RenderFillRect(renderer,&s_taskMenuRect);        
            SDL_FRect s_charDrawRect = {float(s_charRect.x), float(s_charRect.y), float(s_charRect.w), float(s_charRect.h)};
            SDL_RenderTexture(renderer,minicharTex,NULL, &s_charDrawRect);
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
            if (showAchieved){
                SDL_SetRenderDrawColor(renderer,161,157,185,255);
            }else{
                SDL_SetRenderDrawColor(renderer,110,105,148,255);
            }
            SDL_RenderFillRect(renderer,&NotAchieveButton);
            SDL_RenderTexture(renderer,notAchieveTexture,NULL,&NotAchieveButton);
            if (showAchieved){
                SDL_SetRenderDrawColor(renderer,110,105,148,255);
            }else{
                SDL_SetRenderDrawColor(renderer,161,157,185,255);
            }
            SDL_RenderFillRect(renderer,&AchieveButton);
            SDL_RenderTexture(renderer,achieveTexture,NULL,&AchieveButton);
       }
        //フルサイズ画面の描画はここまで



        //タスクの描画
        SDL_FRect taskArea;
        int maxTasks;
        if (isSmall){
            taskArea = s_taskMenuRect;
            maxTasks = 10;
        }else{
            taskArea = TaskRect;
            maxTasks = 10;
        }
        float taskHeight = taskArea.h / maxTasks;
        int taskCount = (int)tasks.size();
        if (taskCount > maxTasks){
            taskCount = maxTasks;
        }
        checkRects.clear();
        checkTaskIds.clear();
        for (int i = 0; i < taskCount; i++){
            SDL_FRect rowRect = {
                taskArea.x, taskArea.y + i * taskHeight,
                taskArea.w, taskHeight
            };
            TTF_Font* taskFont;
            if (isSmall){
                taskFont = sfont;
            }else{
                taskFont = Ffont;
            }
            float boxSize = (float)TTF_GetFontHeight(taskFont);
            SDL_FRect checkRect = {
                rowRect.x + rowRect.x / 10 + 4,
                rowRect.y + rowRect.h / 2 - boxSize / 2,
                boxSize, boxSize
            };
            checkRects.push_back(checkRect);
            checkTaskIds.push_back(tasks[i].id);
            if(isSmall){
                taskCard.DrawTaskCard(renderer,tasks[i],rowRect,sfont);
            }else{
                Task displayTask = tasks[i];
                int charCount = 0;
                while (charCount < 40){
                    displayTask.TaskContent += ' ';
                    charCount++;
                }
                taskCard.DrawTaskCard(renderer,displayTask,rowRect,Ffont);
            }
            
        }

        SDL_RenderPresent(renderer);




        //フレーム固定処理
        uFlames++;
        fFPS = uFlames / (timerCountFPS.getTicks()/ 1000.);
        //ピリオドをつけるとfloatになる。なければint扱い、0で割る可能性. ticksはmsなので1000で割る.

        timeModify.fpsadjust(30, timeModifyFPS);
    }
    //破棄
    SDL_DestroyTexture(notAchieveTexture);
    SDL_DestroyTexture(achieveTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();

    SDL_Quit();

    return 0;
}
