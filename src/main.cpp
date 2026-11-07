#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "SDL_all.h"
#include "SDLFps.hpp"
#include "SDLTimer.hpp"
#include "Window.h"
#include "TaskCard.hpp"
#include "CharacterReaction.hpp"
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
    SDL_Texture *bubbleTexture = nullptr;
    CharacterReaction characterReaction;
    SDL_Color fColor = {0,0,0,255};
    bool isSmall = false;
    bool showAchieved = false;
    bool inputActive = false;
    bool imeConfirmPending = false;
    string newTaskContent;
    string compositionText;
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
    SDL_FRect s_fukidashiRect = {
        s_charRect.x + s_charRect.w + 8, 8,
        SMALL_SCW - (s_charRect.x + s_charRect.w + 8) - 8, SMALL_SCH / 6 - 16
    };
    SDL_FRect fullCharRect = {8,SCREEN_HEIGHT - 180,(float)menuLine - 16,160};
    SDL_FRect fullBubbleRect = {8,SCREEN_HEIGHT - 300,(float)menuLine - 16,110};
    
    SDL_FRect inputRect = {TaskRect.x,TaskRect.y,TaskRect.w,TaskRect.h / 10};
    Uint32 uFlames = 0;
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
    SDL_Surface* bubbleSurface = IMG_Load("assets/fukidashi-dot-rectangle-03-white.png");
    bubbleTexture = SDL_CreateTextureFromSurface(renderer,bubbleSurface);
    SDL_DestroySurface(bubbleSurface);
    SDL_SetTextureScaleMode(bubbleTexture,SDL_SCALEMODE_NEAREST);
    InitCharacterReaction(&characterReaction,renderer,SDL_GetTicks());

    
    //フォントのロード
    Ffont = TTF_OpenFont("assets/BestTen-DOT.otf",FONTSIZE);
    sfont = TTF_OpenFont("assets/BestTen-DOT.otf",SMALL_FONTSIZE);
    fsuf = TTF_RenderText_Blended(sfont,"メニュー",0,fColor);
    ftex = SDL_CreateTextureFromSurface(renderer,fsuf);
    SDL_DestroySurface(fsuf);
    fsuf = TTF_RenderText_Blended(sfont,"未達成",0,fColor);
    notAchieveTexture = SDL_CreateTextureFromSurface(renderer,fsuf);
    SDL_DestroySurface(fsuf);
    fsuf = TTF_RenderText_Blended(sfont,"達成済",0,fColor);
    achieveTexture = SDL_CreateTextureFromSurface(renderer,fsuf);
    SDL_DestroySurface(fsuf);
    
    //ゲームループ
    while (bQuit == false)
   {
        timeModifyFPS.restart();
        if (compositionText.empty()){
            imeConfirmPending = false;
        }
        SDL_PumpEvents();
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT){
                bQuit = true;
            }
            if (event.type == SDL_EVENT_TEXT_EDITING && inputActive){
                compositionText = event.edit.text;
                if (!compositionText.empty()){
                    imeConfirmPending = true;
                }
            }
            if (event.type == SDL_EVENT_TEXT_INPUT && inputActive){
                string inputText = event.text.text;
                for (int i = 0; i < (int)inputText.size(); i++){
                    if (inputText[i] != ',' && inputText[i] != '\n' && inputText[i] != '\r'){
                        newTaskContent += inputText[i];
                    }
                }
                compositionText.clear();
            }
            if (event.type == SDL_EVENT_KEY_UP
                && (event.key.key == SDLK_RETURN || event.key.key == SDLK_KP_ENTER)){
                if (compositionText.empty()){
                    imeConfirmPending = false;
                }
            }
            if (event.type == SDL_EVENT_KEY_DOWN){
                if (event.key.key == SDLK_ESCAPE && !event.key.repeat){
                    if (inputActive){
                        inputActive = false;
                        compositionText.clear();
                        imeConfirmPending = false;
                        SDL_StopTextInput(window);
                    }else{
                        bQuit = true;
                    }
                }else if (inputActive){
                    if (event.key.key == SDLK_RETURN || event.key.key == SDLK_KP_ENTER){
                        if (!event.key.repeat && compositionText.empty() && !imeConfirmPending
                            && !newTaskContent.empty()){
                            if (taskCard.newWriteTaskDB(newTaskContent) == 0){
                                NotifyCharacterAction(&characterReaction,CHARACTER_TASK_ADDED,SDL_GetTicks());
                                newTaskContent.clear();
                            }
                        }
                    }else if (event.key.key == SDLK_BACKSPACE && compositionText.empty()
                        && !imeConfirmPending && !newTaskContent.empty()){
                        int bytePos = (int)newTaskContent.size() - 1;
                        while (bytePos > 0 && ((unsigned char)newTaskContent[bytePos] & 0xC0) == 0x80){
                            bytePos--;
                        }
                        newTaskContent.erase(bytePos);
                    }
                }else if (event.key.key == SDLK_Q && !event.key.repeat){
                    if (!most_front){
                        most_front = true;
                        isSmall = true;
                    }else{
                        most_front = false;
                        isSmall = false;
                    }
                    checkRects.clear();
                    checkTaskIds.clear();
                    if (isSmall){
                        inputRect = {s_taskMenuRect.x,s_taskMenuRect.y,s_taskMenuRect.w,s_taskMenuRect.h / 10};
                    }else{
                        inputRect = {TaskRect.x,TaskRect.y,TaskRect.w,TaskRect.h / 10};
                    }
                }
            }
            if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST && inputActive){
                inputActive = false;
                compositionText.clear();
                imeConfirmPending = false;
                SDL_StopTextInput(window);
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT){
                bool buttonClicked = false;
                if (!showAchieved
                    && event.button.x >= inputRect.x
                    && event.button.x < inputRect.x + inputRect.w
                    && event.button.y >= inputRect.y
                    && event.button.y < inputRect.y + inputRect.h){
                    inputActive = true;
                    SDL_StartTextInput(window);
                    SDL_Rect textInputArea = {
                        (int)inputRect.x, (int)inputRect.y,
                        (int)inputRect.w, (int)inputRect.h
                    };
                    SDL_SetTextInputArea(window,&textInputArea,0);
                    buttonClicked = true;
                }else if (inputActive){
                    inputActive = false;
                    compositionText.clear();
                    imeConfirmPending = false;
                    SDL_StopTextInput(window);
                }
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
                            int id = checkTaskIds[i];
                            Task beforeTask = taskCard.ReadTaskDB(id,false);
                            bool wasUnfinished = (beforeTask.id == id);
                            if (taskCard.flagChange(id) == 0){
                                Task updatedTask = taskCard.ReadTaskDB(id,wasUnfinished);
                                if (updatedTask.id == id){
                                    if (wasUnfinished){
                                        NotifyCharacterAction(&characterReaction,CHARACTER_TASK_COMPLETED,SDL_GetTicks());
                                    }else{
                                        NotifyCharacterAction(&characterReaction,CHARACTER_TASK_REOPENED,SDL_GetTicks());
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
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
        int unfinishedCount = 0;
        for (int i = 0; i < (int)taskIds.size(); i++){
            int id = taskIds[i];
            if (i > 0 && id == taskIds[i - 1]){
                continue;
            }
            Task task = taskCard.ReadTaskDB(id,false);
            if (task.id == id){
                unfinishedCount++;
            }
            if (showAchieved){
                task = taskCard.ReadTaskDB(id,true);
            }
            if (task.id == id){
                tasks.push_back(task);
            }
        }

        UpdateCharacterReaction(&characterReaction,unfinishedCount,SDL_GetTicks());

        if (isSmall){
            SDL_SetRenderDrawColor(renderer,0,0,0,0);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer,110,105,148,255);//線の色
            SDL_RenderFillRect(renderer,&s_taskMenuLineRect);
            SDL_SetRenderDrawColor(renderer,161,157,185,255);
            SDL_RenderFillRect(renderer,&s_taskMenuRect);        
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
        int normalizeLength;
        if (isSmall){
            taskArea = s_taskMenuRect;
            maxTasks = 10;
            normalizeLength = 10;
        }else{
            taskArea = TaskRect;
            maxTasks = 10;
            normalizeLength = 40;
        }
        float taskHeight = taskArea.h / maxTasks;
        int firstTaskRow = 0;
        if (!showAchieved){
            firstTaskRow = 1;
            maxTasks--;
            inputRect = {taskArea.x,taskArea.y,taskArea.w,taskHeight};
            SDL_SetRenderDrawColor(renderer,161,157,185,255);
            SDL_RenderFillRect(renderer,&inputRect);
            if (inputActive){
                SDL_SetRenderDrawColor(renderer,0,0,0,255);
                SDL_RenderRect(renderer,&inputRect);
            }
            string inputLabel = newTaskContent + compositionText;
            if (inputLabel.empty()){
                inputLabel = "タスクを追加……";
            }
            inputLabel = NormalizeText(inputLabel,normalizeLength);
            TTF_Font* inputFont;
            if (isSmall){
                inputFont = sfont;
            }else{
                inputFont = Ffont;
            }
            SDL_Surface* inputSurface = TTF_RenderText_Blended(inputFont,inputLabel.c_str(),0,fColor);
            SDL_Texture* inputTexture = SDL_CreateTextureFromSurface(renderer,inputSurface);
            SDL_DestroySurface(inputSurface);
            SDL_FRect inputTextRect = {inputRect.x + 8,inputRect.y + 5,inputRect.w - 16,inputRect.h - 10};
            SDL_RenderTexture(renderer,inputTexture,NULL,&inputTextRect);
            SDL_DestroyTexture(inputTexture);
        }
        int taskCount = (int)tasks.size();
        if (taskCount > maxTasks){
            taskCount = maxTasks;
        }
        checkRects.clear();
        checkTaskIds.clear();
        for (int i = 0; i < taskCount; i++){
            SDL_FRect rowRect = {
                taskArea.x, taskArea.y + (i + firstTaskRow) * taskHeight,
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
            Task displayTask = tasks[i];
            displayTask.TaskContent = NormalizeText(displayTask.TaskContent,normalizeLength);
            taskCard.DrawTaskCard(renderer,displayTask,rowRect,taskFont);
            
        }

        if (isSmall){
            DrawCharacterReaction(&characterReaction,renderer,bubbleTexture,true,s_charRect,s_fukidashiRect);
        }else{
            DrawCharacterReaction(&characterReaction,renderer,bubbleTexture,false,fullCharRect,fullBubbleRect);
        }
        SDL_RenderPresent(renderer);




        //フレーム固定処理
        uFlames++;
        fFPS = uFlames / (timerCountFPS.getTicks()/ 1000.);
        //ピリオドをつけるとfloatになる。なければint扱い、0で割る可能性. ticksはmsなので1000で割る.

        timeModify.fpsadjust(30, timeModifyFPS);
    }
    //破棄
    SDL_StopTextInput(window);
    DestroyCharacterReaction(&characterReaction);
    SDL_DestroyTexture(bubbleTexture);
    SDL_DestroyTexture(notAchieveTexture);
    SDL_DestroyTexture(achieveTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();

    SDL_Quit();

    return 0;
}
