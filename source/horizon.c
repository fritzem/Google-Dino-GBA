#include "horizon.h"
#include "game.h"
#include "dinoSheetHelper.h"

void updateHorizon(GAME_STATE * gameState) {
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
            updateCloud((horizonState->clouds) + tempCursor);
            tempCursor = (tempCursor + 1) % MAX_CLOUDS;
        }

        if ((horizonState->cloudCount < MAX_CLOUDS &&
             ((SCREEN_WIDTH - ((horizonState->clouds + lastCursor)->xPos)) >
              (horizonState->clouds + lastCursor)->cloudGap)) && randomBool()) {
            addCloud();
        }
    } else {
        addCloud();
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
            setStarPos(starsSet,
                       horizonState->star0X,
                       horizonState->star0Y,
                       horizonState->star1X,
                       horizonState->star1Y
            );
        } else {
            if (horizonState->star0X < -SCREEN_WIDTH)
                horizonState->star0X = SCREEN_WIDTH;
            if (horizonState->star1X < -SCREEN_WIDTH)
                horizonState->star1X = SCREEN_WIDTH;
        }

        if (horizonState->moonMov >= MOON_MOVE_THRESHOLD) {
            horizonState->moonX -= 1;
            horizonState->moonMov = 0;
            setMoonPos(moonSet, horizonState->moonX, MOON_Y);
        } else if (horizonState->moonX < -MOON_WIDTH) {
            horizonState->moonX = SCREEN_WIDTH;
        }
    }

    //obstacles
    if (gameState->spawnObstacles)
        updateObstacles(scrollSpeed, gameState->speed);


    REG_BG0HOFS = horizonState->scroll;
}

void updateObstacles(int scrollSpeed, int gameSpeed) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        OBSTACLE * obs = (horizonState->obstacles + i);
        if (obs->visible) {
            updateObstacle(obs, scrollSpeed, i);
            setObstaclePos(obstacleSets + i, obs->type, obs->size, obs->x, obs->y);
        }
    }

    if (horizonState->obstacleCount > 0) {
        OBSTACLE *lastObs = (horizonState->obstacles + (horizonState->lastObstacle));
        if ((horizonState->obstacleCount < MAX_OBSTACLES) &&
            (((lastObs->gap) + (lastObs->x) + (lastObs->width)) < SCREEN_WIDTH)) {
            addObstacle(gameSpeed);
        }
    } else {
        addObstacle(gameSpeed);
    }
}

void addObstacle(int speed) {
    OBSTACLE *obs = (OBSTACLE*)(horizonState->obstacles + (horizonState->obstacleCursor));
    switch (qran_range(0,(OBSTACLE_TYPES - (speed < DACTYL_MIN_SPEED)))) {
        case CACTUS_SMALL:
            createCactusSmall(obs, speed);
            setObstacleSet(obstacleSets + horizonState->obstacleCursor, CACTUS_SMALL, obs->size);
            break;
        case CACTUS_LARGE:
            createCactusLarge(obs, speed);
            setObstacleSet(obstacleSets + horizonState->obstacleCursor, CACTUS_LARGE, obs->size);
            break;
        case PTERODACTYL:
            createPterodactyl(obs, speed);
            setObstacleSet(obstacleSets + horizonState->obstacleCursor, PTERODACTYL, obs->size);
            break;
    }
    setObstaclePos(obstacleSets + (horizonState->obstacleCursor), obs->type, obs->size, obs->x, obs->y);
    horizonState->obstacleCount += 1;
    horizonState->lastObstacle = horizonState->obstacleCursor;
    horizonState->obstacleCursor += 1;
    if (horizonState->obstacleCursor >= MAX_OBSTACLES)
        horizonState->obstacleCursor = 0;
}

void updateNight() {
    if (!(horizonState->night) && meterState->invertCounter >= INVERT_DISTANCE) {
        meterState->invertCounter -= INVERT_DISTANCE;
        horizonState->night = true;
        horizonState->inverting = true;
        horizonState->fading = true;
        placeStars();
        horizonState->moonPhase = incrementMoonPhase(moonSet, horizonState->moonPhase);
    } else if ((horizonState->night) && horizonState->invertTimer >= INVERT_FADE_DURATION) {
        horizonState->invertTimer = 0;
        horizonState->night = false;
        horizonState->inverting = true;
        horizonState->fading = true;
    } else if (horizonState->night) {
        horizonState->invertTimer += 1;
    }
}

void placeStars() {
    horizonState->star0X = qran_range(0, SCREEN_WIDTH / 2);
    horizonState->star0Y = qran_range(0, STAR_MAX_Y);
    horizonState->star1X = qran_range(SCREEN_WIDTH / 2, SCREEN_WIDTH);
    horizonState->star1Y = qran_range(0, STAR_MAX_Y);

    setStarPos(starsSet,
               horizonState->star0X,
               horizonState->star0Y,
               horizonState->star1X,
               horizonState->star1Y
    );
}

void addCloud() {
    int newCursor = (horizonState->cloudCursor + horizonState->cloudCount) % MAX_CLOUDS;
    horizonState->cloudCount += 1;


    initCloud(((horizonState->clouds) + newCursor), newCursor);
}

void updateCloud(CLOUD * cloud) {
    cloud->xPos -= 1;
    setCloudPos((clouds + (cloud->cloudNum)), cloud->xPos, cloud->yPos);
    if (!cloudVisible(cloud)) {
        horizonState->cloudCursor = (horizonState->cloudCursor + 1) % MAX_CLOUDS;
        horizonState->cloudCount -= 1;
    }
}

bool cloudVisible(CLOUD * cloud) {
    return ((cloud->xPos) > -CLOUD_WIDTH);
}