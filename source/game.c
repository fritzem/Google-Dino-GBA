#include "game.h"
#include "tonc.h"
#include "dino.h"
#include "dinoSheetHelper.h"
#include <maxmod.h>
#include <string.h>
#include <dino_soundbank.h>
#include "obstacle.h"
#include "save.h"
#include "dinoSheet.h"
#include <palette_bank.h>

//Ethan Fritz 2021, 2025

extern const byte dino_soundbank_bin;
const char saveType[] __attribute__((aligned(4))) = "SRAM_Vnnn";

OBJ_ATTR obj_buffer[128];

typedef struct {
    GAME_STATE gameState;
    DINO_STATE dinoState;
    HORIZON_STATE horizonState;
    METER_STATE meterState;
} STATE;

void input(STATE * state);
void updateGame(STATE * state);
void gameOver(GAME_STATE * gameState, METER_STATE * meterState);

STATE * initGame();
void resetGame(STATE * state);

void init();
void initSound();
void initMem();
void initGraphics();


int main() {
    init();

    STATE * state = initGame();

    // Ensure it's not optimized away
    char y = saveType[0]; y = y;

    while(1) {
        mmFrame();
        VBlankIntrWait();

        input(state);
        updateGame(state);

        oam_copy(oam_mem, obj_buffer, 64);
    }
}

void input(STATE * state) {
    key_poll();
    inputDino(&state->dinoState, &state->gameState);

    if (JUMP_HIT && !state->gameState.playing) {
        hideTitle(titleSet);
        dinoUnBlink(dinoSet);
    } else if (JUMP_RELEASED) {
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
            dinoGraphicsUpdate(dinoState, dinoSet);
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
    if (DINO_ANIMATING) setDinoAnim(dinoSet, dinoState->animSI[dinoState->frame]);

    dinoGraphicsUpdate(dinoState, dinoSet);
}

void gameOver(GAME_STATE * gameState, METER_STATE * meterState) {
    mmEffect(SFX_HIT);
    setDinoCrashed(dinoSet);

    gameState->playing = false;

    toggleReplayHide(replaySet);
    showGameover(gameoverSet);

    gameState->gameoverFrames = 0;

    if (meterState->distance > readHiscore()) {
        setHiscore(meterState->distance);
        setNumValue(hiScoreSet, meterState->distance);
    }
    setNumValue(scoreSet, meterState->distance);
    showNum(scoreSet);
}

STATE * initGame() {
    STATE * state = calloc(sizeof(STATE), 1);
    initState(&state->gameState);
    initDino(&state->dinoState);

    initHorizon(&state->horizonState);

    initMeter(&state->meterState);
    if (readHiscore() == -1)
        setHiscore(0);
    setNumValue(hiScoreSet, readHiscore());
    sqran(readHiscore());

    return state;
}

void resetGame(STATE * state) {
    toggleReplayHide(replaySet);
    hideGameover(gameoverSet);

    resetState(&state->gameState);
    resetDino(&state->dinoState);
    resetHorizon(&state->horizonState);
    resetObstacles(state->horizonState.obstacles);
    initMeter(&state->meterState);

    setDinoUpright(dinoSet);
    dinoGraphicsUpdate(&state->dinoState, dinoSet);

    wipeObstacleSet(obstacleSets);
    wipeObstacleSet(obstacleSets + 1);
    showNum(scoreSet);
}

void init() {
    initSound();
    initMem();
    initGraphics();
}

void initSound() {
    IRQ_INIT();

    irq_add(II_VBLANK, mmVBlank);
    irq_enable(II_VBLANK);
    mmInitDefault((mm_addr)&dino_soundbank_bin, 8);
}

void initMem() {
    memcpy(&tile_mem[4][0], dinoSheetTiles, dinoSheetTilesLen);
    memcpy(&tile_mem[0][0], dinoSheetTiles, dinoSheetTilesLen);

    memcpy(pal_obj_mem, defaultPal, defaultPalLen);
    memcpy(pal_bg_mem, defaultPal, defaultPalLen);
}

void initGraphics() {
    whiteOutBG();
    backgroundInit();

    //horizon layer
    REG_BG0CNT = BG_PRIO(3) | BG_CBB(0) | BG_SBB(31) | BG_4BPP | BG_REG_32x32;
    //curtain layer
    REG_BG1CNT = BG_PRIO(0) | BG_CBB(0) | BG_SBB(29) | BG_4BPP | BG_REG_64x32;

    oam_init(obj_buffer, 128);
    initSets();
    assembleSets();

    REG_DISPCNT= DCNT_OBJ | DCNT_OBJ_2D | DCNT_BG0 | DCNT_BG1;
    REG_BLDCNT = BLD_BUILD(BLD_OBJ, BLD_BG0, BLD_STD);
    REG_BLDALPHA = BLDA_BUILD(0, 16);
    REG_BLDY= BLDY_BUILD(16);
}