#ifndef TaskCard_hpp
#define TaskCard_hpp

#include "SDL_all.h"

class TaskCard{
protected:
   
public:
    TaskCard();
    virtual ~TaskCard();
    //DB操作
    Task ReadTaskDB(int id,bool flags);
    int newWriteTaskDB(string taskContent);
    int flagChange(int id);
    //描画処理
    int DrawTaskCard(SDL_Renderer* renderer, Task task, SDL_FRect TaskRect);
};



#endif /* TaskCard.hpp*/