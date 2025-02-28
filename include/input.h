#ifndef DINO_GBA_INPUT_H
#define DINO_GBA_INPUT_H

#include "tonc.h"
#include "state.h"

#define BIT_HIT_UP 0x1
#define BIT_HIT_DOWN 0x2
#define BIT_RELEASE_UP 0x4
#define BIT_RELEASE_DOWN 0x8
extern u32 dino_input;

#define HIT_UP (dino_input & BIT_HIT_UP)
#define HIT_DOWN (dino_input & BIT_HIT_DOWN)
#define RELEASE_UP (dino_input & BIT_RELEASE_UP)
#define RELEASE_DOWN (dino_input & BIT_RELEASE_DOWN)

void checkModeOverride(STATE * state);
void inputP1(STATE * state);
void inputP2(STATE * state);

#endif //DINO_GBA_INPUT_H