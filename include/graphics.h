#ifndef DINO_GBA_GRAPHICS_H
#define DINO_GBA_GRAPHICS_H

#include "tonc.h"
#include "dinoSheetHelper.h"

typedef struct {
    struct {
        u32 x;
        u32 y;
        u32 width;
        u32 height;
    } viewport;
    OBJ_ATTR * baseOAM;
} GRAPHICS_CTX;

GRAPHICS_CTX *initGraphics();

void drawGame(GRAPHICS_CTX *ctx, STATE *state);

// No clue why ATTR0_Y/ATTR0_X doesn't include these masks
#define Y(y) ((y) & ATTR0_Y_MASK)
#define X(x) ((x) & ATTR1_X_MASK)

#endif //DINO_GBA_GRAPHICS_H