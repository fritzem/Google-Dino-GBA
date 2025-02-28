#include "horizon.h"
#include "dinoSheetHelper.h"
#include "dino.h"

#define MIN_CLOUD_GAP 100
#define MAX_CLOUD_GAP 400
#define MIN_SKY_LEVEL 71
#define MAX_SKY_LEVEL 30

#define STAR_MAX_Y 70
#define STAR_SPEED 3
#define STAR_MOVE_THRESHOLD 10

#define FADE_SKY_FRAMES 29

#define MOON_SPEED 25
#define MOON_MOVE_THRESHOLD 100
#define MOON_WIDTH 40

#define INVERT_DISTANCE 700
#define INVERT_FADE_DURATION 720

bool randomBool();

void initCloud(CLOUD * cloud, int cloudNum);
bool updateCloud(CLOUD * cloud);

void placeStars(HORIZON_STATE * horizonState);

void updateObstacles(STATE * state, HORIZON_STATE * horizonState, int scrollSpeed);
void addObstacle(HORIZON_STATE * horizonState, int speed, bool canSpawnRevive);

void updateHorizon(STATE * state, HORIZON_STATE * horizonState) {
    GAME_STATE * gameState = &state->gameState;
    int scrollSpeed = (gameState->speed + horizonState->extraScroll);
    int scrolled =  scrollSpeed >> SPEED_POINT_DIV;
    horizonState->extraScroll = (gameState->speed + horizonState->extraScroll) & SPEED_REM;
    horizonState->scroll += scrolled;
    horizonState->scrolled += scrolled;

    //horizon terrain
    if (horizonState->scrolled >= TILE_SIZE) {

        horizonState->terrainScroll += (horizonState->scrolled >> 3);
        if (horizonState->terrainScroll >= TERRAIN_STRIP_LENGTH) {
            horizonState->bumpy = randomBool();
            horizonState->terrainScroll %= TERRAIN_STRIP_LENGTH;
        }

        for (int i = 0; i < (horizonState->scrolled >> 3); i++) {
            updateHorizonTile(horizonState->nextScrollTile, horizonState->terrainScroll, horizonState->bumpy);
            horizonState->nextScrollTile++;
            horizonState->nextScrollTile &= 0x1F;
        }

        horizonState->scrolled &= 0x7;
    }

    //clouds
    if (horizonState->cloudCount) {
        int tempCursor = horizonState->cloudCursor;
        int lastCursor = 0;
        int preCount = horizonState->cloudCount;
        for (int i = 0; i < preCount; i++) {
            lastCursor = tempCursor;
            if (updateCloud((horizonState->clouds) + tempCursor)) {
                horizonState->cloudCursor = (horizonState->cloudCursor + 1) % MAX_CLOUDS;
                horizonState->cloudCount -= 1;
            }
            tempCursor = (tempCursor + 1) % MAX_CLOUDS;
        }

        if ((horizonState->cloudCount < MAX_CLOUDS &&
             ((SCREEN_WIDTH - ((horizonState->clouds + lastCursor)->xPos)) >
              (horizonState->clouds + lastCursor)->cloudGap)) && randomBool()) {
            addCloud(horizonState);
        }
    } else {
        addCloud(horizonState);
    }

    //night
    if (horizonState->inverting) {
        horizonState->invertFrame += 1;
        bool invertOver = horizonState->invertFrame >= INVERT_FRAMES;
        inversionUpdate(horizonState->night, invertOver);

        if (invertOver) {
            horizonState->inverting = false;
            horizonState->invertFrame = 0;
        }
    }

    if (horizonState->fading) {
        int truOpac = horizonState->opacity / 2;
        REG_BLDALPHA = BLDA_BUILD(truOpac, 16 - truOpac);
        REG_BLDY= BLDY_BUILD(16 - truOpac);

        if (horizonState->night)
            horizonState->opacity += 1;
        else
            horizonState->opacity -= 1;

        if (horizonState->opacity == 32 || horizonState->opacity == 0)
            horizonState->fading = false;
    }

    if (horizonState->night || horizonState->fading) {
        horizonState->starMov += STAR_SPEED;
        horizonState->moonMov += MOON_SPEED;

        if (horizonState->starMov >= STAR_MOVE_THRESHOLD) {
            horizonState->starMov -= STAR_MOVE_THRESHOLD;
            horizonState->star0X -= 1;
            horizonState->star1X -= 1;
        } else {
            if (horizonState->star0X < -SCREEN_WIDTH)
                horizonState->star0X = SCREEN_WIDTH;
            if (horizonState->star1X < -SCREEN_WIDTH)
                horizonState->star1X = SCREEN_WIDTH;
        }

        if (horizonState->moonMov >= MOON_MOVE_THRESHOLD) {
            horizonState->moonX -= 1;
            horizonState->moonMov = 0;
        } else if (horizonState->moonX < -MOON_WIDTH) {
            horizonState->moonX = SCREEN_WIDTH;
        }
    }

    //obstacles
    if (gameState->spawnObstacles)
        updateObstacles(state, horizonState, scrollSpeed);


    REG_BG0HOFS = horizonState->scroll;
}

void updateObstacles(STATE * state, HORIZON_STATE * horizonState, int scrollSpeed) {
    int gameSpeed = state->gameState.speed;

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        OBSTACLE * obs = (horizonState->obstacles + i);
        if (obs->visible) {
            horizonState->obstacleCount -= updateObstacle(obs, scrollSpeed, i);;
        }
    }

    bool canRevive = state->mode == COOP
            && (state->dinoState.status == CRASHED || state->minoState.status == CRASHED);

    if (horizonState->obstacleCount > 0) {
        OBSTACLE *lastObs = (horizonState->obstacles + (horizonState->lastObstacle));
        if ((horizonState->obstacleCount < MAX_OBSTACLES) &&
            (((lastObs->gap) + (lastObs->x) + (lastObs->width)) < SCREEN_WIDTH)) {
            addObstacle(horizonState, gameSpeed, canRevive);
        }
    } else {
        addObstacle(horizonState, gameSpeed, canRevive);
    }
}

void addObstacle(HORIZON_STATE * horizonState, int speed, bool canSpawnRevive) {
    OBSTACLE *obs = (OBSTACLE*)(horizonState->obstacles + (horizonState->obstacleCursor));

    bool reviveInPool = canSpawnRevive && (qran() > 0x6800);

    switch (qran_range(true ^ reviveInPool,(OBSTACLE_TYPE_COUNT - (speed < DACTYL_MIN_SPEED)))) {
        case REVIVE:
            createRevive(obs, speed);
            break;
        case CACTUS_SMALL:
            createCactusSmall(obs, speed);
            break;
        case CACTUS_LARGE:
            createCactusLarge(obs, speed);
            break;
        case PTERODACTYL:
            createPterodactyl(obs, speed);
            break;
    }
    horizonState->obstacleCount += 1;
    horizonState->lastObstacle = horizonState->obstacleCursor;
    horizonState->obstacleCursor += 1;
    if (horizonState->obstacleCursor >= MAX_OBSTACLES)
        horizonState->obstacleCursor = 0;
}

void updateNight(HORIZON_STATE * horizonState, METER_STATE * meterState) {
    if (!(horizonState->night) && meterState->invertCounter >= INVERT_DISTANCE) {
        meterState->invertCounter -= INVERT_DISTANCE;
        horizonState->night = true;
        horizonState->inverting = true;
        horizonState->fading = true;
        placeStars(horizonState);
        horizonState->moonPhase++;
        horizonState->moonPhase -= (horizonState->moonPhase == 7) * 7;
    } else if ((horizonState->night) && horizonState->invertTimer >= INVERT_FADE_DURATION) {
        horizonState->invertTimer = 0;
        horizonState->night = false;
        horizonState->inverting = true;
        horizonState->fading = true;
    } else if (horizonState->night) {
        horizonState->invertTimer += 1;
    }
}

void placeStars(HORIZON_STATE * horizonState) {
    horizonState->star0X = qran_range(0, SCREEN_WIDTH / 2);
    horizonState->star0Y = qran_range(0, STAR_MAX_Y);
    horizonState->star1X = qran_range(SCREEN_WIDTH / 2, SCREEN_WIDTH);
    horizonState->star1Y = qran_range(0, STAR_MAX_Y);
}

bool randomBool() {
    return ((qran_range(0, 1000) > 500) ? true : false);
}

void initHorizon(HORIZON_STATE * horizon) {
    horizon->nextScrollTile = 31;
    horizon->terrainScroll = 31;
    horizon->bumpy = false;

    horizon->clouds = malloc(MAX_CLOUDS * sizeof(CLOUD));

    horizon->night = false;
    horizon->inverting = false;
    horizon->fading = false;

    horizon->moonPhase = -1;
    horizon->moonX = SCREEN_WIDTH - 50;

    horizon->obstacles = malloc(MAX_OBSTACLES * sizeof(OBSTACLE));
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        (horizon->obstacles + i)->colBox = malloc(MAX_HITBOXES * sizeof(COLLISION_BOX));
        (horizon->obstacles + i)->visible = false;
    }
}

void resetHorizon(HORIZON_STATE * horizon) {
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

void initCloud(CLOUD * cloud, int cloudNum) {
    cloud->xPos = SCREEN_WIDTH;
    cloud->yPos = qran_range(MIN_SKY_LEVEL, MAX_SKY_LEVEL);
    cloud->cloudGap = qran_range(MIN_CLOUD_GAP, MAX_CLOUD_GAP);
    cloud->cloudNum = cloudNum;
}

void addCloud(HORIZON_STATE * horizonState) {
    int newCursor = (horizonState->cloudCursor + horizonState->cloudCount) % MAX_CLOUDS;
    horizonState->cloudCount += 1;
    initCloud(((horizonState->clouds) + newCursor), newCursor);
}

// Return true if cloud is no longer visible
bool updateCloud(CLOUD * cloud) {
    cloud->xPos -= 1;
    return ((cloud->xPos) <= -CLOUD_WIDTH);
}