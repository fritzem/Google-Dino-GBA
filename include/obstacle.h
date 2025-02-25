#ifndef DINO_GBA_OBSTACLE_H
#define DINO_GBA_OBSTACLE_H

#include "hitbox.h"

#define CACTUS_SMALL 0
#define CACTUS_LARGE 1
#define PTERODACTYL  2

#define CACTUS_SMALL_Y 113
#define CACTUS_SMALL_Y_SPRITE_OFFSET 5
#define CACTUS_SMALL_WIDTH 17
#define CACTUS_SMALL_HEIGHT 35
#define CACTUS_SMALL_MULTI_SPEED 4
#define CACTUS_LARGE_Y 103
#define CACTUS_LARGE_WIDTH 25
#define CACTUS_LARGE_HEIGHT 50
#define CACTUS_LARGE_MULTI_SPEED 6
#define CACTUS_GAP 120 * SPEED_POINT
#define DACTYL_WIDTH 46
#define DACTYL_HEIGHT 40
#define DACTYL_GAP 150 * SPEED_POINT
#define DACTYL_FRAMES 10
#define DACTYL_MIN_SPEED 6000
#define DACTYL_SPEED_OFFSET 800

typedef struct OBSTACLE {
    int type;
    int x;
    int y;
    int size;
    int width;
    int height;
    int gap;
    int speedOffset;
    bool visible;

    int frames;
    int extraSpeed;

    int numBoxes;
    COLLISION_BOX * colBox;
    int spriteY;
} OBSTACLE, OBSTACLE;

INLINE void resetObstacles(OBSTACLE * obs) {
    obs->visible = 0;
    (obs + 1)->visible = 0;
}

void createCactusSmall(OBSTACLE * obs);
void createCactusLarge(OBSTACLE * obs);
void createPterodactyl(OBSTACLE * obs);
void updateObstacle(OBSTACLE * obs, int scrollSpeed, int index);

#endif //DINO_GBA_OBSTACLE_H