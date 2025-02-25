#include "meter.h"
#include "dinoSheetHelper.h"

#define ACHIEVEMENT_DISTANCE 100
#define FLASH_FRAMES 15
#define FLASH_ITERATIONS 3

int distanceConvert(int distance);

bool updateDistanceMeter(METER_STATE * meterState, int distance) {
    int trueDistance = distanceConvert(distance);
    int deltaDistance = (trueDistance - meterState->distance);
    meterState->invertCounter += deltaDistance;
    meterState->achievementCounter += deltaDistance;
    meterState->distance = trueDistance;
    if (meterState->achieving) {
        if (meterState->flashIteration <= FLASH_ITERATIONS) {
            meterState->flashFrame += 1;
            if (meterState->flashFrame == 1) {
                hideNum(scoreSet);
            } else if (meterState->flashFrame == FLASH_FRAMES * 2) {
                meterState->flashFrame = 0;
                meterState->flashIteration += 1;
                showNum(scoreSet);
            } else if (meterState->flashFrame == FLASH_FRAMES) {
                showNum(scoreSet);
            }
        } else {
            meterState->flashIteration = 0;
            meterState->flashFrame = 0;
            meterState->achieving = false;
        }
    } else {
        if (meterState->achievementCounter >= ACHIEVEMENT_DISTANCE) {
            meterState->achievementCounter -= ACHIEVEMENT_DISTANCE;
            meterState->achieving = true;
            setNumValue(scoreSet, trueDistance);
            return true;
        } else {
            setNumValue(scoreSet, trueDistance);
        }
    }

    return false;
}

int distanceConvert(int distance) {
    return distance / 40; // (* 0.025)
}

