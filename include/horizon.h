#ifndef DINO_GBA_HORIZON_H
#define DINO_GBA_HORIZON_H

#include <malloc.h>
#include "tonc.h"
#include "obstacle.h"
#include "dino.h"
#include "state.h"

#define OBSTACLE_TYPES 3
#define MAX_OBSTACLES 2
#define MAX_OBSTACLE_SIZE 3

#define MAX_CLOUDS 6
#define MIN_CLOUD_GAP 100
#define MAX_CLOUD_GAP 400
#define MIN_SKY_LEVEL 71
#define MAX_SKY_LEVEL 30
#define STAR_MAX_Y 70
#define STAR_SPEED 3
#define STAR_MOVE_THRESHOLD 10
#define FADE_SKY_FRAMES 29
#define MOON_Y 30
#define MOON_SPEED 25
#define MOON_MOVE_THRESHOLD 100
#define MOON_WIDTH 40

#define INVERT_DISTANCE 700
#define INVERT_FRAMES 90
#define INVERT_FADE_DURATION 720

typedef struct CLOUD {
    int xPos;
    int yPos;
    int cloudGap;
    int cloudNum;
} CLOUD, CLOUD;

INLINE void initCloud(CLOUD * cloud, int cloudNum) {
    cloud->xPos = SCREEN_WIDTH;
    cloud->yPos = qran_range(MIN_SKY_LEVEL, MAX_SKY_LEVEL);
    cloud->cloudGap = qran_range(MIN_CLOUD_GAP, MAX_CLOUD_GAP);
    cloud->cloudNum = cloudNum;
}

void updateObstacles(int scrollSpeed, int gameSpeed);
void addObstacle(int speed);

void addCloud();
void updateCloud(CLOUD * cloud);
bool cloudVisible(CLOUD * cloud);

typedef struct HORIZON_STATE {
    int scroll;
    int nextScrollTile;
    int scrolled;
    int terrainScroll;
    bool bumpy;
    int extraScroll;

    CLOUD * clouds;
    int cloudCursor;
    int cloudCount;

    int invertTimer;
    int invertFrame;
    int fadeFrame;
    int opacity;
    bool night;
    bool inverting;
    bool fading;

    int star0X;
    int star0Y;
    int star1X;
    int star1Y;
    int starMov;

    int moonPhase;
    int moonX;
    int moonMov;

    OBSTACLE * obstacles;
    int obstacleCount;
    int obstacleCursor;
    int lastObstacle;
} HORIZON_STATE, HORIZON_STATE;

INLINE void initHorizon(HORIZON_STATE * horizon) {
    horizon->scroll = 0;
    horizon->nextScrollTile = 31;
    horizon->scrolled = 0;
    horizon->terrainScroll = 31;
    horizon->bumpy = false;
    horizon->extraScroll = 0;

    horizon->cloudCursor = 0;
    horizon->cloudCount = 0;
    horizon->clouds = malloc(MAX_CLOUDS * sizeof(CLOUD));

    horizon->night = false;
    horizon->inverting = false;
    horizon->fading = false;
    horizon->invertTimer = 0;
    horizon->invertFrame = 0;
    horizon->opacity = 0;

    horizon->star0X = 0;
    horizon->star0Y = 0;
    horizon->star1X = 0;
    horizon->star1Y = 0;
    horizon->starMov = 0;

    horizon->moonPhase = -1;
    horizon->moonX = SCREEN_WIDTH - 50;
    horizon->moonMov = 0;

    horizon->obstacles = malloc(MAX_OBSTACLES * sizeof(OBSTACLE));
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        (horizon->obstacles + i)->colBox = malloc(MAX_HITBOXES * sizeof(COLLISION_BOX));
    }

    horizon->obstacleCount = 0;
    horizon->obstacleCursor = 0;
    horizon->lastObstacle = 0;
}

INLINE void resetHorizon(HORIZON_STATE * horizon) {
    horizon->scroll = 0;
    horizon->nextScrollTile = 31;
    horizon->scrolled = 0;
    horizon->terrainScroll = 31;
    horizon->bumpy = false;
    horizon->extraScroll = 0;

    if (horizon->night) {
        horizon->night = false;
        horizon->inverting = true;
        horizon->fading = true;
    }

    horizon->obstacleCount = 0;
    horizon->obstacleCursor = 0;
    horizon->lastObstacle = 0;
}

void updateNight();
void updateHorizon(GAME_STATE * gameState);
void placeStars();

#endif //DINO_GBA_HORIZON_H