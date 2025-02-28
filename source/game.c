#include "game.h"
#include "tonc.h"
#include "dino.h"
#include <maxmod.h>
#include <dino_soundbank.h>
#include "obstacle.h"
#include "save.h"
#include "graphics.h"
#include "input.h"

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
    inputP1(state);

    if (!state->gameState.startedPlaying) {
        // Start game, pass input to Dino
        if (HIT_UP) {
            inputDino(&state->dinoState, &state->gameState);
            return;
        }
        checkModeOverride(state);
    } else if (state->gameState.gameOver) {
        if (RELEASE_UP) {
            if (state->dinoState.status == CRASHED && state->gameState.gameOverFrames >= RESET_FRAMES) {
                resetGame(state);
            }
        }
    } else {
        inputDino(&state->dinoState, &state->gameState);
        if (state->mode == COOP) {
            inputP2(state);
            inputDino(&state->minoState, &state->gameState);
        }
    }
}

void updateGame(STATE * state) {
    // #using state
    GAME_STATE * gameState = &state->gameState;
    DINO_STATE * dinoState = &state->dinoState;
    DINO_STATE * minoState = &state->minoState;
    HORIZON_STATE * horizonState = &state->horizonState;
    METER_STATE * meterState = &state->meterState;

    updateDino(dinoState);
    if (state->mode == COOP) {
        updateDino(minoState);
    }
    if (!gameState->startedPlaying) {
        if (dinoState->status == RUNNING || dinoState->status == DUCKING) {
            // First landing, start intro
            startGame(state);
        } else {
            // Waiting at title
            gameState->randoFrames += 1;
            return;
        }
    } else if (gameState->gameOver) {
        gameState->gameOverFrames += 1;
        return;
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
        updateHorizon(state, horizonState);
    }

    OBSTACLE * collided = collisionCheck(dinoState, horizonState);
    bool collision = gameState->spawnObstacles && collided && dinoState->status != CRASHED;

    if (!collision) {
        addPoint(gameState->speed, &gameState->distanceRan, &gameState->distanceRanPoint);

        if (gameState->speed < SPEED_MAX)
            gameState->speed += ACCELERATION;
    } else {
        if (state->mode == COOP && collided->type == REVIVE) {
            despawnObstacle(collided);
            mmEffect(SFX_SCORE_REACHED);
            if (minoState->status == CRASHED)
                dinoRevive(minoState);
        } else {
            mmEffect(SFX_HIT);
            dinoCrash(dinoState);
        }
    }

    if (state->mode == COOP) {
        // Check for mino collision, decide Co-Op game over
        collided = collisionCheck(minoState, horizonState);
        collision = gameState->spawnObstacles && collided && minoState->status != CRASHED;
        if (collision) {
            if (collided->type == REVIVE) {
                despawnObstacle(collided);
                mmEffect(SFX_SCORE_REACHED);
                if (dinoState->status == CRASHED)
                    dinoRevive(dinoState);
            } else {
                mmEffect(SFX_HIT);
                dinoCrash(minoState);
            }
        }
        if (state->dinoState.status == CRASHED && state->minoState.status == CRASHED) {
            gameOver(gameState, meterState);
        }
    } else {
        // Regular game over check
        if (state->dinoState.status == CRASHED) {
            gameOver(gameState, meterState);
        }
    }

    if (updateDistanceMeter(meterState, (gameState->distanceRan) + ((gameState->distanceRanPoint) ? 1 : 0)))
        mmEffect(SFX_SCORE_REACHED);

    updateNight(horizonState, meterState);
}

void gameOver(GAME_STATE * gameState, METER_STATE * meterState) {
    gameState->gameOver = true;
    gameState->gameOverFrames = 0;
    meterState->flashing = false;
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

    state->mode = NORMAL;
    return state;
}

void startGame(STATE * state) {
    state->gameState.playingIntro = true;
    state->gameState.startedPlaying = true;
    sqran(state->gameState.randoFrames);
    addCloud(&state->horizonState);
}

void resetGame(STATE * state) {
    resetState(&state->gameState);
    resetDino(&state->dinoState);
    if (state->mode == COOP) {
        resetDino(&state->minoState);
    }
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