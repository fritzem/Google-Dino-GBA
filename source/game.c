#include "game.h"
#include "tonc.h"
#include "dino.h"
#include "graphics/dinoSheetHelper.h"
#include <maxmod.h>
#include <dino_soundbank.h>
#include "obstacle.h"
#include "save.h"
#include "graphics/graphics.h"

//Ethan Fritz 2021, 2025

extern const byte dino_soundbank_bin;
const char saveType[] __attribute__((aligned(4))) = "SRAM_Vnnn";

void input(STATE * state);
void updateGame(STATE * state);
void gameOver(GAME_STATE * gameState, METER_STATE * meterState);

STATE * initGame();
void resetGame(STATE * state);

void initSound();

int main() {
    key_poll();

    initSound();
    GRAPHICS_CTX *graphicsCtx = initGraphics();

    STATE * state = initGame();

    // Ensure it's not optimized away
    char y = saveType[0]; y = y;

    while(1) {
        mmFrame();
        VBlankIntrWait();

        input(state);
        updateGame(state);
        drawGame(graphicsCtx, state);
    }
}

void input(STATE * state) {
    key_poll();
    inputDino(&state->dinoState, &state->gameState);

    if (JUMP_RELEASED) {
        if (state->dinoState.status == CRASHED && state->gameState.gameoverFrames >= RESET_FRAMES) {
            resetGame(state);
        }
    }

}

void updateGame(STATE * state) {
    // #using state
    GAME_STATE * gameState = &state->gameState;
    DINO_STATE * dinoState = &state->dinoState;
    HORIZON_STATE * horizonState = &state->horizonState;
    METER_STATE * meterState = &state->meterState;

    updateDino(dinoState);
    if (!gameState->playing) {
        if (dinoState->status == RUNNING) {
            // First landing, start intro
            gameState->playingIntro = true;
            gameState->playing = true;
            sqran(gameState->randoFrames);
            addCloud(horizonState);
        } else {
            // Waiting at title
            gameState->randoFrames += 1;
            gameState->gameoverFrames += 1;
            return;
        }

    }

    //If 3 second grace period expires, spawn obstacles
    //Accounts for overflow if you somehow make it 400 or so days
    gameState->runningFrames += 1;
    gameState->spawnObstacles = (gameState->runningFrames > CLEAR_FRAMES)
                                || gameState->spawnObstacles;

    if (gameState->playingIntro) {
        gameState->curtainScroll -= 8;
        if (gameState->curtainScroll <= 256) {
            gameState->curtainScroll = 256;
            gameState->playingIntro = false;
        }
        REG_BG1HOFS = gameState->curtainScroll;
    } else {
        updateHorizon(horizonState, gameState);
    }

    bool collision = gameState->spawnObstacles &&
                     collisionCheck(dinoState, horizonState);

    if (!collision) {
        addPoint(gameState->speed, &gameState->distanceRan, &gameState->distanceRanPoint);

        if (gameState->speed < SPEED_MAX)
            gameState->speed += ACCELERATION;
    } else {
        dinoState->status = CRASHED;
        gameOver(gameState, meterState);
    }

    if (updateDistanceMeter(meterState, (gameState->distanceRan) + ((gameState->distanceRanPoint) ? 1 : 0)))
        mmEffect(SFX_SCORE_REACHED);

    updateNight(horizonState, meterState);

    dinoState->frameCounter += 1;
    if ((DINO_ANIMATING) && (dinoState->frameCounter >= dinoState->frameTime)) {
        dinoState->frame = dinoState->frame == 1 ? 0 : 1;
        dinoState->frameCounter = 0;
    }
}

void gameOver(GAME_STATE * gameState, METER_STATE * meterState) {
    mmEffect(SFX_HIT);
    gameState->playing = false;
    gameState->gameoverFrames = 0;
    if (meterState->distance > readHiscore()) {
        setHiscore(meterState->distance);
        gameState->hiScore = meterState->distance;
    }
}

STATE * initGame() {
    STATE * state = calloc(sizeof(STATE), 1);
    initState(&state->gameState);
    initDino(&state->dinoState);
    initHorizon(&state->horizonState);
    initMeter(&state->meterState);

    if (readHiscore() == -1)
        setHiscore(0);
    state->gameState.hiScore = readHiscore();
    sqran(state->gameState.hiScore);

    return state;
}

void resetGame(STATE * state) {
    resetState(&state->gameState);
    resetDino(&state->dinoState);
    resetHorizon(&state->horizonState);
    resetObstacles(state->horizonState.obstacles);
    initMeter(&state->meterState);
}

void initSound() {
    IRQ_INIT();

    irq_add(II_VBLANK, mmVBlank);
    irq_enable(II_VBLANK);
    mmInitDefault((mm_addr)&dino_soundbank_bin, 8);
}