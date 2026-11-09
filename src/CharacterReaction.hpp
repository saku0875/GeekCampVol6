#ifndef CharacterReaction_hpp
#define CharacterReaction_hpp

#include "SDL_all.h"
#include <vector>

enum CharacterPose {
    CHARACTER_NORMAL,
    CHARACTER_HAPPY,
    CHARACTER_CONCERNED,
    CHARACTER_SLEEPING,
    CHARACTER_POSE_COUNT
};

enum CharacterAction {
    CHARACTER_TASK_ADDED,
    CHARACTER_TASK_COMPLETED,
    CHARACTER_TASK_REOPENED
};

struct CharacterReaction {
    CharacterPose pose;
    CharacterAction pendingAction;
    bool hasPendingAction;
    bool pendingMultiple;
    bool idleMessageShown;
    Uint64 lastProgressTime;
    Uint64 speechUntil;
    std::vector<Uint64> completionTimes;
    string speech;

    SDL_Texture* fullTextures[CHARACTER_POSE_COUNT];
    SDL_Texture* smallTextures[CHARACTER_POSE_COUNT];
    SDL_FRect fullSources[CHARACTER_POSE_COUNT];
    SDL_FRect smallSources[CHARACTER_POSE_COUNT];
    TTF_Font* speechFont;
    SDL_Texture* speechTextures[2];
    string cachedSpeech[2];
    int cachedWrapWidth[2];
    float speechWidths[2];
    float speechHeights[2];
};

void InitCharacterReaction(CharacterReaction* reaction, SDL_Renderer* renderer, Uint64 now);
void NotifyCharacterAction(CharacterReaction* reaction, CharacterAction action, Uint64 now);
void UpdateCharacterReaction(CharacterReaction* reaction, int unfinishedCount, Uint64 now);
void DrawCharacterReaction(CharacterReaction* reaction, SDL_Renderer* renderer,
    SDL_Texture* bubbleTexture, bool isSmall, SDL_FRect characterRect, SDL_FRect bubbleRect);
void DestroyCharacterReaction(CharacterReaction* reaction);

#endif
