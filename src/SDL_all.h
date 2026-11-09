#ifndef SDL_all_h_
#define SDL_all_h_

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include "Function.h"
#include <iostream>
# include <fstream>
# include <sstream>
#define SCREEN_WIDTH 640*2
#define SCREEN_HEIGHT 480*2
#define SMALL_SCW 300 
#define SMALL_SCH 600
#define FONTSIZE 20
#define SMALL_FONTSIZE 32

using std::string;
using std::ifstream;
using std::ofstream;
using std::istringstream;

struct Task{
    int id;
    int date;
    string TaskContent;
    bool flags;
};
typedef struct Task Task;

#endif /* SDL_all_h_ */
