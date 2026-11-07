#include "CharacterReaction.hpp"
#include <cstdio>
#include <cstdlib>

static void Check(bool condition, const char* expression, int line) {
    if (!condition){
        std::fprintf(stderr,"Line %d: %s\n",line,expression);
        std::exit(1);
    }
}

#define CHECK(expression) Check((expression),#expression,__LINE__)

static bool IsOneOf(const string& text, const char* const choices[], int count) {
    for (int i = 0; i < count; i++){
        if (text == choices[i]) return true;
    }
    return false;
}

static const char* single[] = {"やるじゃん", "いいね", "えら", "天才"};
static const char* multiple[] = {"まじ？", "えぐ", "神やん", "最高"};
static const char* all[] = {"全部終わったー！", "流石に惚れる", "お疲れ様"};
static const char* added[] = {"がんばれー", "おお", "すげ"};
static const char* manyAdded[] = {"頑張りすぎじゃない？", "めっちゃあるやん", "大丈夫？？", "終わる？？"};
static const char* idle[] = {"つかれた", "だるい", "もう寝よ"};

static void TestActions() {
    CharacterReaction reaction = {};
    UpdateCharacterReaction(&reaction,0,0);
    CHECK(reaction.pose == CHARACTER_SLEEPING && reaction.speech.empty());
    NotifyCharacterAction(&reaction,CHARACTER_TASK_ADDED,1000);
    UpdateCharacterReaction(&reaction,4,1000);
    CHECK(IsOneOf(reaction.speech,added,3));
    CHECK(reaction.pose == CHARACTER_NORMAL);
    string firstSpeech = reaction.speech;
    UpdateCharacterReaction(&reaction,4,4999);
    CHECK(reaction.speech == firstSpeech);
    UpdateCharacterReaction(&reaction,4,5000);
    CHECK(reaction.speech.empty());
    NotifyCharacterAction(&reaction,CHARACTER_TASK_ADDED,6000);
    UpdateCharacterReaction(&reaction,5,6000);
    CHECK(IsOneOf(reaction.speech,manyAdded,4));
    CHECK(reaction.pose == CHARACTER_CONCERNED);
    NotifyCharacterAction(&reaction,CHARACTER_TASK_COMPLETED,7000);
    UpdateCharacterReaction(&reaction,4,7000);
    CHECK(IsOneOf(reaction.speech,single,4));
    NotifyCharacterAction(&reaction,CHARACTER_TASK_COMPLETED,8000);
    UpdateCharacterReaction(&reaction,3,8000);
    CHECK(IsOneOf(reaction.speech,single,4));
    NotifyCharacterAction(&reaction,CHARACTER_TASK_COMPLETED,9000);
    UpdateCharacterReaction(&reaction,2,9000);
    CHECK(IsOneOf(reaction.speech,multiple,4));
    CHECK(reaction.completionTimes.empty());
    NotifyCharacterAction(&reaction,CHARACTER_TASK_COMPLETED,10000);
    UpdateCharacterReaction(&reaction,1,10000);
    CHECK(IsOneOf(reaction.speech,single,4));
    NotifyCharacterAction(&reaction,CHARACTER_TASK_COMPLETED,11000);
    UpdateCharacterReaction(&reaction,0,11000);
    CHECK(IsOneOf(reaction.speech,all,3));
    CHECK(reaction.pose == CHARACTER_HAPPY && reaction.completionTimes.empty());
    UpdateCharacterReaction(&reaction,0,16999);
    CHECK(!reaction.speech.empty());
    UpdateCharacterReaction(&reaction,0,17000);
    CHECK(reaction.speech.empty() && reaction.pose == CHARACTER_SLEEPING);
    NotifyCharacterAction(&reaction,CHARACTER_TASK_REOPENED,18000);
    UpdateCharacterReaction(&reaction,1,18000);
    CHECK(reaction.speech.empty() && reaction.pose == CHARACTER_NORMAL);
    CHECK(reaction.lastProgressTime == 11000);
}

static void TestCompletionWindow() {
    for (int outside = 0; outside < 2; outside++){
        CharacterReaction reaction = {};
        NotifyCharacterAction(&reaction,CHARACTER_TASK_COMPLETED,100);
        UpdateCharacterReaction(&reaction,5,100);
        NotifyCharacterAction(&reaction,CHARACTER_TASK_COMPLETED,150100);
        UpdateCharacterReaction(&reaction,4,150100);
        NotifyCharacterAction(&reaction,CHARACTER_TASK_COMPLETED,300100 + outside);
        UpdateCharacterReaction(&reaction,3,300100 + outside);
        if (outside){
            CHECK(IsOneOf(reaction.speech,single,4));
        }else{
            CHECK(IsOneOf(reaction.speech,multiple,4));
        }
    }
    CharacterReaction reaction = {};
    NotifyCharacterAction(&reaction,CHARACTER_TASK_COMPLETED,100);
    NotifyCharacterAction(&reaction,CHARACTER_TASK_COMPLETED,101);
    NotifyCharacterAction(&reaction,CHARACTER_TASK_COMPLETED,102);
    UpdateCharacterReaction(&reaction,0,102);
    CHECK(IsOneOf(reaction.speech,all,3));
    CHECK(reaction.completionTimes.empty());
    NotifyCharacterAction(&reaction,CHARACTER_TASK_COMPLETED,103);
    NotifyCharacterAction(&reaction,CHARACTER_TASK_ADDED,104);
    UpdateCharacterReaction(&reaction,1,104);
    CHECK(IsOneOf(reaction.speech,added,3));
    CHECK(reaction.completionTimes.size() == 1);
}

static void TestIdle() {
    CharacterReaction reaction = {};
    reaction.lastProgressTime = 1000;
    UpdateCharacterReaction(&reaction,5,1000);
    CHECK(reaction.speech.empty() && reaction.pose == CHARACTER_CONCERNED);
    UpdateCharacterReaction(&reaction,5,300999);
    CHECK(reaction.speech.empty());
    UpdateCharacterReaction(&reaction,5,301000);
    CHECK(IsOneOf(reaction.speech,idle,3));
    CHECK(reaction.pose == CHARACTER_NORMAL && reaction.idleMessageShown);
    UpdateCharacterReaction(&reaction,5,305000);
    CHECK(reaction.speech.empty());
    UpdateCharacterReaction(&reaction,5,900999);
    CHECK(reaction.speech.empty() && reaction.pose == CHARACTER_NORMAL);
    UpdateCharacterReaction(&reaction,5,901000);
    CHECK(reaction.speech.empty() && reaction.pose == CHARACTER_SLEEPING);
    NotifyCharacterAction(&reaction,CHARACTER_TASK_REOPENED,902000);
    UpdateCharacterReaction(&reaction,6,902000);
    CHECK(reaction.lastProgressTime == 1000 && reaction.pose == CHARACTER_SLEEPING);
    NotifyCharacterAction(&reaction,CHARACTER_TASK_ADDED,903000);
    UpdateCharacterReaction(&reaction,7,903000);
    CHECK(!reaction.idleMessageShown && reaction.lastProgressTime == 903000);
    CHECK(reaction.pose == CHARACTER_CONCERNED);
    UpdateCharacterReaction(&reaction,7,1203000);
    CHECK(IsOneOf(reaction.speech,idle,3));
    CharacterReaction empty = {};
    UpdateCharacterReaction(&empty,0,300000);
    CHECK(empty.speech.empty() && empty.pose == CHARACTER_SLEEPING);
    CharacterReaction skipped = {};
    UpdateCharacterReaction(&skipped,2,900000);
    CHECK(skipped.speech.empty() && skipped.pose == CHARACTER_SLEEPING);
}

static bool PixelIs(SDL_Surface* surface, int x, int y, int r, int g, int b) {
    Uint8 red, green, blue, alpha;
    CHECK(SDL_ReadSurfacePixel(surface,x,y,&red,&green,&blue,&alpha));
    return red == r && green == g && blue == b && alpha == 255;
}

static void TestDrawing(const char* previewPath) {
    CHECK(SDL_Init(0));
    CHECK(TTF_Init());
    SDL_Surface* canvas = SDL_CreateSurface(1280,960,SDL_PIXELFORMAT_RGBA32);
    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(canvas);
    CHECK(canvas != NULL && renderer != NULL);
    CharacterReaction reaction;
    InitCharacterReaction(&reaction,renderer,0);
    SDL_Surface* bubbleSurface = IMG_Load("assets/fukidashi-dot-rectangle-03-white.png");
    CHECK(bubbleSurface != NULL);
    CHECK(PixelIs(bubbleSurface,300,120,255,255,255));
    Uint8 r, g, b, a;
    SDL_ReadSurfacePixel(bubbleSurface,0,0,&r,&g,&b,&a);
    CHECK(a == 0);
    SDL_Texture* bubble = SDL_CreateTextureFromSurface(renderer,bubbleSurface);
    SDL_DestroySurface(bubbleSurface);
    SDL_FRect character = {8,780,240,160};
    SDL_FRect balloon = {8,660,240,110};
    for (int small = 0; small < 2; small++){
        if (small){
            character = {10,10,100,90};
            balloon = {118,8,174,84};
        }
        for (int pose = 0; pose < CHARACTER_POSE_COUNT; pose++){
            CHECK(reaction.fullTextures[pose] != NULL && reaction.smallTextures[pose] != NULL);
            reaction.pose = (CharacterPose)pose;
            for (int message = 0; message < 4; message++){
                SDL_SetRenderDrawColor(renderer,10,60,90,255);
                SDL_RenderClear(renderer);
                reaction.speech = manyAdded[message];
                DrawCharacterReaction(&reaction,renderer,bubble,small != 0,character,balloon);
                SDL_RenderPresent(renderer);
                CHECK(PixelIs(canvas,(int)(balloon.x + balloon.w / 2),(int)(balloon.y + balloon.h * 0.7f),255,255,255));
                CHECK(PixelIs(canvas,(int)balloon.x,(int)balloon.y,10,60,90));
                CHECK(reaction.speechTextures[small] != NULL);
                CHECK(reaction.cachedSpeech[small] == reaction.speech);
                CHECK(reaction.speechHeights[small] <= balloon.h * 0.8f - 24);
            }
        }
        SDL_SetRenderDrawColor(renderer,10,60,90,255);
        SDL_RenderClear(renderer);
        reaction.speech.clear();
        DrawCharacterReaction(&reaction,renderer,bubble,small != 0,character,balloon);
        SDL_RenderPresent(renderer);
        CHECK(PixelIs(canvas,(int)(balloon.x + balloon.w / 2),(int)(balloon.y + balloon.h / 2),10,60,90));
    }
    if (previewPath != NULL){
        SDL_SetRenderDrawColor(renderer,186,183,204,255);
        SDL_RenderClear(renderer);
        for (int pose = 0; pose < CHARACTER_POSE_COUNT; pose++){
            float x = (float)(pose * 320);
            SDL_FRect largeCharacter = {x + 20,170,280,180};
            SDL_FRect largeBubble = {x + 40,40,240,110};
            SDL_FRect miniCharacter = {x + 10,400,100,90};
            SDL_FRect miniBubble = {x + 118,408,174,84};
            reaction.pose = (CharacterPose)pose;
            reaction.speech = added[0];
            if (pose == CHARACTER_HAPPY) reaction.speech = all[0];
            if (pose == CHARACTER_CONCERNED) reaction.speech = manyAdded[0];
            if (pose == CHARACTER_SLEEPING) reaction.speech.clear();
            DrawCharacterReaction(&reaction,renderer,bubble,false,largeCharacter,largeBubble);
            DrawCharacterReaction(&reaction,renderer,bubble,true,miniCharacter,miniBubble);
        }
        SDL_RenderPresent(renderer);
        CHECK(IMG_SavePNG(canvas,previewPath));
    }
    DestroyCharacterReaction(&reaction);
    SDL_DestroyTexture(bubble);
    SDL_DestroyRenderer(renderer);
    SDL_DestroySurface(canvas);
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    std::srand(1);
    TestActions();
    TestCompletionWindow();
    TestIdle();
    const char* previewPath = NULL;
    if (argc > 1) previewPath = argv[1];
    TestDrawing(previewPath);
    std::puts("PASS: actions, phrase pools, completion windows, idle boundaries, existing sprites and speech rendering");
    return 0;
}
