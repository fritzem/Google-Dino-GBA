#ifndef DINO_GBA_OBSTACLE_H
#define DINO_GBA_OBSTACLE_H

#include "hitbox.h"

#define MAX_CACTUS_SIZE 3

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

typedef enum {
    CACTUS_SMALL,
    CACTUS_LARGE,
    PTERODACTYL,
    OBSTACLE_TYPE_COUNT
} OBSTACLE_TYPE;

typedef struct OBSTACLE {
    OBSTACLE_TYPE type;
    int x;
    int y;
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

    union {
        int cactusSize;
        bool flap;
        int typeCategory;
    };
} OBSTACLE, OBSTACLE;

INLINE void resetObstacles(OBSTACLE * obs) {
    obs->visible = 0;
    (obs + 1)->visible = 0;
}

void createCactusSmall(OBSTACLE * obs, int speed);
void createCactusLarge(OBSTACLE * obs, int speed);
void createPterodactyl(OBSTACLE * obs, int speed);
bool updateObstacle(OBSTACLE * obs, int scrollSpeed, int index);

#endif //DINO_GBA_OBSTACLE_H