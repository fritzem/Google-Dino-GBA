#ifndef DINO_GBA_GAME_H
#define DINO_GBA_GAME_H

#include "tonc.h"
#include "dino.h"
#include "horizon.h"
#include "meter.h"
#include "state.h"

// Delay until game over screen accepts input
#define RESET_FRAMES 45

INLINE void initState(GAME_STATE * state) {
    state->speed = SPEED;
    state->curtainScroll = STARTING_CURTAIN_SCROLL;
    state->distanceRan = 0;
    state->distanceRanPoint = 0;

    state->runningFrames = 0;
    state->randoFrames = 0;
    state->spawnObstacles = false;

    state->playing = false;
    state->playingIntro = false;

    state->gameoverFrames = 0;
}

INLINE void resetState(GAME_STATE * state) {
    state->speed = SPEED;
    state->distanceRan = 0;
    state->distanceRanPoint = 0;

    state->runningFrames = 0;
    state->spawnObstacles = false;

    state->playing = true;
}

extern OBJ_ATTR obj_buffer[];

#endif //DINO_GBA_GAME_H