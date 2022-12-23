#ifndef __GameState__
#define __GameState__

#include "GameStateUtils.h"

enum GameState{
    ERROR = -1,
    INTRO = 0,
    VOTING = 1,
    ADMIN_PANEL = 2,
    GAME = 3,
    STATUS = 4
};

#endif // __GameState__

