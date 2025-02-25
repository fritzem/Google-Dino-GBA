#include "obstacle.h"
#include "dino.h"
#include "dinoSheetHelper.h"

/**
 *
 * @param obs Obstacle to update
 * @param scrollSpeed
 * @param index
 * @return True if obstacle is out of play.
 */
bool updateObstacle(OBSTACLE * obs, int scrollSpeed, int index) {
    int speedOffset = (obs->speedOffset + obs->extraSpeed) >> SPEED_POINT_DIV;
    obs->extraSpeed = (obs->speedOffset + obs->extraSpeed) & SPEED_REM;

    obs->x -= (scrollSpeed >> SPEED_POINT_DIV) + speedOffset;
    obs->visible = (obs->x > -(obs->width));
    if (!(obs->visible)) {
        return true;
    }

    if (obs->type == PTERODACTYL) {
        obs->frames += 1;
        if (obs->frames == DACTYL_FRAMES) {
            obs->frames = 0;
            obs->flap = toggleDactylFlap(obstacleSets + index, obs->flap);
        }
    }

    return false;
}

const COLLISION_BOX cactSmallBoxes[] = {
        {0, 7, 5, 27},
        {4, 0, 6, 34},
        {10, 4, 7, 14}
};

const COLLISION_BOX cactLargeBoxes[] = {
        {0, 12, 7, 38},
        {8, 0, 7, 49},
        {13, 10, 10, 38}
};

const COLLISION_BOX pterodactylBoxes[] = {
        {15, 15, 16, 5},
        {18, 21, 24, 6},
        {2, 14, 4, 3},
        {6, 10, 4, 7},
        {10, 8, 6, 9}
};

void createCactusSmall(OBSTACLE * obs, int speed) {
    obs->type = CACTUS_SMALL;
    obs->x = SCREEN_WIDTH;
    obs->y = CACTUS_SMALL_Y;
    obs->cactusSize = (speed >> SPEED_POINT_DIV >= CACTUS_SMALL_MULTI_SPEED) ?
                      qran_range(1, MAX_CACTUS_SIZE + 1) : 1;
    obs->width = CACTUS_SMALL_WIDTH * obs->cactusSize;
    obs->height = CACTUS_SMALL_HEIGHT;
    obs->gap = qran_range(((speed) * (obs->width) + (CACTUS_GAP / 10 * 6)) >> SPEED_POINT_DIV,
                          ((CACTUS_GAP + CACTUS_GAP / 2) >> SPEED_POINT_DIV) + 1);
    obs->speedOffset = 0;
    obs->visible = true;

    obs->extraSpeed = 0;

    obs->numBoxes = CACT_COLLISION_BOXES;
    obs->spriteY = obs->y + CACTUS_SMALL_Y_SPRITE_OFFSET;
    cloneBox(obs->colBox, cactSmallBoxes, obs->numBoxes);
    adjustBox(obs->colBox, obs->cactusSize, obs->width);
}

void createCactusLarge(OBSTACLE * obs, int speed) {
    obs->type = CACTUS_LARGE;
    obs->x = SCREEN_WIDTH;
    obs->y = CACTUS_LARGE_Y;
    obs->cactusSize = (speed >> SPEED_POINT_DIV >= CACTUS_LARGE_MULTI_SPEED) ?
                      qran_range(1, MAX_CACTUS_SIZE + 1) : 1;
    obs->width = CACTUS_LARGE_WIDTH * obs->cactusSize;
    obs->height = CACTUS_LARGE_HEIGHT;
    obs->gap = qran_range((obs->width * speed + (CACTUS_GAP / 10 * 6)) >> SPEED_POINT_DIV,
                          ((CACTUS_GAP + CACTUS_GAP / 2) >> SPEED_POINT_DIV) + 1);
    obs->speedOffset = 0;
    obs->visible = true;

    obs->extraSpeed = 0;

    obs->numBoxes = CACT_COLLISION_BOXES;
    obs->spriteY = CACTUS_LARGE_Y;
    cloneBox(obs->colBox, cactLargeBoxes, obs->numBoxes);
    adjustBox(obs->colBox, obs->cactusSize, obs->width);
}

const int dactylHeights[3] = {100,75,50};

void createPterodactyl(OBSTACLE * obs, int speed) {
    obs->type = PTERODACTYL;
    obs->x = SCREEN_WIDTH;
    obs->y = dactylHeights[qran_range(0,3)];
    obs->flap = false;
    obs->width = DACTYL_WIDTH;
    obs->height = DACTYL_HEIGHT;
    obs->gap = qran_range((obs->width * speed + (DACTYL_GAP / 10 * 6)) >> SPEED_POINT_DIV,
                          ((DACTYL_GAP + DACTYL_GAP / 2) >> SPEED_POINT_DIV) + 1);
    obs->speedOffset = (qran_range(0,2)) ? DACTYL_SPEED_OFFSET : -DACTYL_SPEED_OFFSET;
    obs->visible = true;

    obs->frames = 0;
    obs->extraSpeed = 0;

    obs->spriteY = obs->y;

    obs->numBoxes = DACTYL_COLLISION_BOXES;
    cloneBox(obs->colBox, pterodactylBoxes, obs->numBoxes);
}