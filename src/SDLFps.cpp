#include "SDLFps.hpp"
#include "SDLTimer.hpp"

SDLFps::SDLFps(){
    uTimepass = 0;
}
SDLFps::~SDLFps(){

}

void SDLFps::fpsadjust(int goalfps, SDLTimer timeModifyFPS){
    uTimepass = timeModifyFPS.getTicks();
    if (uTimepass < 1000/goalfps ){
        SDL_Delay(1000/goalfps - uTimepass);
    }
}