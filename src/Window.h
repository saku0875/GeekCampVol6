#ifndef WINDOW_H
#define WINDOW_H
#include "SDL_all.h"
#include "SDLFps.hpp"
#include "SDLTimer.hpp"

int SetWindow(SDL_Window* window, bool isSmall);
SDL_HitTestResult SmallWindow_Drag(SDL_Window *window,const SDL_Point *area,void *callback_data );


/*int drawWindow(SDL_Renderer* renderer, SDL_Texture *ftex, SDL_bool isSmall,SDL_Rect TaskRect, SDL_Rect MenuRect, SDL_Rect MenuBox,int menuLine){
    if (isSmall){


    }else{
        SDL_SetRenderDrawColor(renderer,161,157,185,127);
        SDL_RenderFillRect(renderer,&MenuRect);        
        SDL_SetRenderDrawColor(renderer,110,105,148,255);//線の色
        SDL_RenderDrawLine(renderer,menuLine,0,menuLine,SCREEN_HEIGHT);
        SDL_RenderDrawLine(renderer,0,SCREEN_HEIGHT/10,menuLine,SCREEN_HEIGHT/10);
        SDL_RenderDrawLine(renderer,0,SCREEN_HEIGHT/5,menuLine,SCREEN_HEIGHT/5);
        SDL_RenderDrawLine(renderer,0,SCREEN_HEIGHT/10*3,menuLine,SCREEN_HEIGHT/10*3);
        SDL_RenderDrawRect(renderer,&TaskRect);
        SDL_RenderCopy(renderer,ftex,NULL, &MenuBox);
    }

    return 0;
}*/

#endif

