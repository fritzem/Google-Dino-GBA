#ifndef DINO_GBA_HORIZON_H
#define DINO_GBA_HORIZON_H

#include <malloc.h>
#include "tonc.h"
#include "obstacle.h"
#include "state.h"
#include "meter.h"

#define MAX_OBSTACLES 2
#define MAX_CLOUDS 6

#define INVERT_FRAMES 90

#define MOON_Y 30

typedef struct CLOUD {
    int xPos;
    int yPos;
    int cloudGap;
    int cloudNum;
} CLOUD, CLOUD;

void updateNight(HORIZON_STATE * horizonState, METER_STATE * meterState);
void updateHorizon(HORIZON_STATE * horizonState, GAME_STATE * gameState);

void addCloud(HORIZON_STATE * horizonState);

void initHorizon(HORIZON_STATE * horizon);
void resetHorizon(HORIZON_STATE * horizon);

#endif //DINO_GBA_HORIZON_H