#ifndef DINO_GBA_METER_H
#define DINO_GBA_METER_H

#include "tonc.h"
#include "state.h"

INLINE void initMeter(METER_STATE * meter) {
    meter->distance = 0;
    meter->invertCounter = 0;
    meter->achievementCounter = 0;
    meter->achieving = false;
    meter->flashFrame = 0;
    meter->flashIteration = 0;
}

bool updateDistanceMeter(METER_STATE * meterState, int distance);

#endif //DINO_GBA_METER_H