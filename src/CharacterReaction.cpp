#include "CharacterReaction.hpp"
#include <cstdlib>
#include <ctime>

static const Uint64 FIVE_MINUTES = 5 * 60 * 1000;
static const Uint64 FIFTEEN_MINUTES = 15 * 60 * 1000;

static const char* singleMessages[] = {"やるじゃん", "いいね", "えら", "天才"};
static const char* multipleMessages[] = {"まじ？", "えぐ", "神やん", "最高"};
static const char* allMessages[] = {"全部終わったー！", "流石に惚れる", "お疲れ様"};
static const char* addedMessages[] = {"がんばれー", "おお", "すげ"};
static const char* manyAddedMessages[] = {
    "頑張りすぎじゃない？", "めっちゃあるやん", "大丈夫？？", "終わる？？"
};
static const char* idleMessages[] = {"つかれた", "だるい", "もう寝よ"};

static SDL_Texture* LoadCharacterTexture(SDL_Renderer* renderer, const char* path, SDL_FRect* source) {
    SDL_Surface* surface = IMG_Load(path);
    int left = surface->w;
    int top = surface->h;
    int right = 0;
    int bottom = 0;
    // 素材の透明な余白を除いて、キャラ本体を描画枠に収める。
    for (int y = 0; y < surface->h; y++){
        for (int x = 0; x < surface->w; x++){
            Uint8 r, g, b, a;
            SDL_ReadSurfacePixel(surface,x,y,&r,&g,&b,&a);
            if (a != 0){
                if (x < left) left = x;
                if (y < top) top = y;
                if (x > right) right = x;
                if (y > bottom) bottom = y;
            }
        }
    }
    *source = {(float)left,(float)top,(float)(right - left + 1),(float)(bottom - top + 1)};
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
    SDL_DestroySurface(surface);
    SDL_SetTextureScaleMode(texture,SDL_SCALEMODE_NEAREST);
    return texture;
}

void InitCharacterReaction(CharacterReaction* reaction, SDL_Renderer* renderer, Uint64 now) {
    *reaction = CharacterReaction();
    reaction->lastProgressTime = now;
    std::srand((unsigned int)std::time(NULL));
    const char* fullPaths[CHARACTER_POSE_COUNT] = {
        "assets/camp_vol.6_2__.png",
        "assets/camp_vol.6_2___.png",
        "assets/camp_vol.6_4_.png",
        "assets/camp_vol.6_2___sleep.png"
    };
    const char* smallPaths[CHARACTER_POSE_COUNT] = {
        "assets/minikyaraTest.png",
        "assets/camp_vol.6___1.png",
        "assets/minikyaraTest.png",
        "assets/camp_vol.6___.png"
    };
    for (int i = 0; i < CHARACTER_POSE_COUNT; i++){
        reaction->fullTextures[i] = LoadCharacterTexture(renderer,fullPaths[i],&reaction->fullSources[i]);
        reaction->smallTextures[i] = LoadCharacterTexture(renderer,smallPaths[i],&reaction->smallSources[i]);
    }
    reaction->speechFont = TTF_OpenFont("assets/BestTen-DOT.otf",16);
}

static void ExpireCompletions(CharacterReaction* reaction, Uint64 now) {
    while (!reaction->completionTimes.empty()
        && now - reaction->completionTimes[0] > FIVE_MINUTES){
        reaction->completionTimes.erase(reaction->completionTimes.begin());
    }
}

void NotifyCharacterAction(CharacterReaction* reaction, CharacterAction action, Uint64 now) {
    reaction->pendingAction = action;
    reaction->hasPendingAction = true;
    reaction->pendingMultiple = false;
    if (action == CHARACTER_TASK_ADDED || action == CHARACTER_TASK_COMPLETED){
        reaction->lastProgressTime = now;
        reaction->idleMessageShown = false;
    }
    if (action == CHARACTER_TASK_COMPLETED){
        ExpireCompletions(reaction,now);
        reaction->completionTimes.push_back(now);
        if (reaction->completionTimes.size() >= 3){
            reaction->pendingMultiple = true;
            reaction->completionTimes.clear();
        }
    }
}

static void Say(CharacterReaction* reaction, const char* const messages[], int count,
    CharacterPose pose, Uint64 now, Uint64 duration) {
    reaction->speech = messages[std::rand() % count];
    reaction->pose = pose;
    reaction->speechUntil = now + duration;
}

void UpdateCharacterReaction(CharacterReaction* reaction, int unfinishedCount, Uint64 now) {
    ExpireCompletions(reaction,now);
    bool hadAction = reaction->hasPendingAction;
    if (hadAction){
        reaction->hasPendingAction = false;
        reaction->speech.clear();
        reaction->speechUntil = 0;
        if (reaction->pendingAction == CHARACTER_TASK_ADDED){
            if (unfinishedCount >= 5){
                Say(reaction,manyAddedMessages,4,CHARACTER_CONCERNED,now,4000);
            }else{
                Say(reaction,addedMessages,3,CHARACTER_NORMAL,now,4000);
            }
        }else if (reaction->pendingAction == CHARACTER_TASK_COMPLETED){
            if (unfinishedCount == 0){
                Say(reaction,allMessages,3,CHARACTER_HAPPY,now,6000);
                reaction->completionTimes.clear();
            }else if (reaction->pendingMultiple){
                Say(reaction,multipleMessages,4,CHARACTER_HAPPY,now,4000);
            }else{
                Say(reaction,singleMessages,4,CHARACTER_HAPPY,now,4000);
            }
        }
    }
    if (!reaction->speech.empty() && now < reaction->speechUntil){
        return;
    }
    reaction->speech.clear();
    Uint64 idleTime = now - reaction->lastProgressTime;
    if (unfinishedCount == 0 || idleTime >= FIFTEEN_MINUTES){
        reaction->pose = CHARACTER_SLEEPING;
    }else if (idleTime >= FIVE_MINUTES){
        reaction->pose = CHARACTER_NORMAL;
        if (!hadAction && !reaction->idleMessageShown){
            Say(reaction,idleMessages,3,CHARACTER_NORMAL,now,4000);
            reaction->idleMessageShown = true;
        }
    }else if (unfinishedCount >= 5){
        reaction->pose = CHARACTER_CONCERNED;
    }else{
        reaction->pose = CHARACTER_NORMAL;
    }
}

static SDL_FRect FitImage(float width, float height, SDL_FRect area) {
    float scale = area.w / width;
    if (height * scale > area.h){
        scale = area.h / height;
    }
    SDL_FRect result = {
        area.x + (area.w - width * scale) / 2,
        area.y + (area.h - height * scale) / 2,
        width * scale, height * scale
    };
    return result;
}

void DrawCharacterReaction(CharacterReaction* reaction, SDL_Renderer* renderer,
    SDL_Texture* bubbleTexture, bool isSmall, SDL_FRect characterRect, SDL_FRect bubbleRect) {
    int sizeIndex = 0;
    SDL_Texture* texture = reaction->fullTextures[reaction->pose];
    SDL_FRect source = reaction->fullSources[reaction->pose];
    if (isSmall){
        sizeIndex = 1;
        texture = reaction->smallTextures[reaction->pose];
        source = reaction->smallSources[reaction->pose];
    }
    SDL_FRect destination = FitImage(source.w,source.h,characterRect);
    SDL_RenderTexture(renderer,texture,&source,&destination);
    if (reaction->speech.empty()){
        return;
    }
    SDL_RenderTexture(renderer,bubbleTexture,NULL,&bubbleRect);
    SDL_FRect textArea = {bubbleRect.x + 12,bubbleRect.y + 12,bubbleRect.w - 24,bubbleRect.h * 0.8f - 24};
    int wrapWidth = (int)textArea.w;
    if (reaction->cachedSpeech[sizeIndex] != reaction->speech
        || reaction->cachedWrapWidth[sizeIndex] != wrapWidth){
        SDL_DestroyTexture(reaction->speechTextures[sizeIndex]);
        SDL_Color color = {0,0,0,255};
        SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(reaction->speechFont,
            reaction->speech.c_str(),0,color,wrapWidth);
        reaction->speechWidths[sizeIndex] = (float)surface->w;
        reaction->speechHeights[sizeIndex] = (float)surface->h;
        reaction->speechTextures[sizeIndex] = SDL_CreateTextureFromSurface(renderer,surface);
        SDL_DestroySurface(surface);
        reaction->cachedSpeech[sizeIndex] = reaction->speech;
        reaction->cachedWrapWidth[sizeIndex] = wrapWidth;
    }
    // 短いセリフは拡大せず、長い場合だけ枠内に縮小する。
    float width = reaction->speechWidths[sizeIndex];
    float height = reaction->speechHeights[sizeIndex];
    SDL_FRect textRect = {textArea.x + (textArea.w - width) / 2,
        textArea.y + (textArea.h - height) / 2,width,height};
    if (width > textArea.w || height > textArea.h){
        textRect = FitImage(width,height,textArea);
    }
    SDL_RenderTexture(renderer,reaction->speechTextures[sizeIndex],NULL,&textRect);
}

void DestroyCharacterReaction(CharacterReaction* reaction) {
    for (int i = 0; i < CHARACTER_POSE_COUNT; i++){
        SDL_DestroyTexture(reaction->fullTextures[i]);
        SDL_DestroyTexture(reaction->smallTextures[i]);
    }
    for (int i = 0; i < 2; i++){
        SDL_DestroyTexture(reaction->speechTextures[i]);
    }
    TTF_CloseFont(reaction->speechFont);
}
