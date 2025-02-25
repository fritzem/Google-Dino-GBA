#ifndef DINO_GBA_HITBOX_H
#define DINO_GBA_HITBOX_H

#include "tonc.h"

typedef struct COLLISION_BOX {
    s32 x;
    s32 y;
    s32 w;
    s32 h;
} COLLISION_BOX, COLLISION_BOX;

void adjustBox(COLLISION_BOX * box, int size, int width);
void cloneBox(COLLISION_BOX * clone, const COLLISION_BOX * base, int boxes);
bool boxCheck(COLLISION_BOX * a, COLLISION_BOX * b);
bool boxCheckOffset(const COLLISION_BOX * a, COLLISION_BOX * b,
                    int x1, int y1, int x2, int y2);

#endif //DINO_GBA_HITBOX_H