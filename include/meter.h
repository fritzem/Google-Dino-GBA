#ifndef DINO_GBA_METER_H
#define DINO_GBA_METER_H

#include "tonc.h"

typedef struct METER_STATE {
    int distance;
    int invertCounter;
    int achievementCounter;
    int flashFrame;
    int flashIteration;
    bool achieving;
} METER_STATE, METER_STATE;

INLINE void initMeter(METER_STATE * meter) {
    meter->distance = 0;
    meter->invertCounter = 0;
    meter->achievementCounter = 0;
    meter->achieving = false;
    meter->flashFrame = 0;
    meter->flashIteration = 0;
}

bool updateDistanceMeter(int distance);
int distanceConvert(int distance);
bool randomBool();

#endif //DINO_GBA_METER_H