#ifndef SDLFps_hpp
#define SDLFps_hpp

#include "SDL_all.h"

class SDLTimer;

class SDLFps{
protected:
    Uint32 uTimepass;

public:
    SDLFps();
    virtual ~SDLFps();
    void fpsadjust(int goalfps,SDLTimer timeModifyFPS);

};







#endif /* SDLFps.hpp*/