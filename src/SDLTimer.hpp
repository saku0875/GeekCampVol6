#ifndef SDLTimer_hpp
#define SDLTimer_hpp

#include "SDL_all.h"

class SDLTimer{
protected:
    Uint32 m_uStartTicks;
public:
    SDLTimer();
    virtual ~SDLTimer();
    Uint32 getTicks() {return SDL_GetTicks() - m_uStartTicks;} //今のtick数と最初のtick数の差で経過時間を取得
    void restart() {m_uStartTicks = SDL_GetTicks();} //最初のtick数を保存


};
//SDL_GetTicksを2度呼び、その差をとることで経過時間を取得する。


#endif /* SDLTimer.hpp*/