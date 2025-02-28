#include <maxmod.h>
#include <dino_soundbank.h>
#include "input.h"
#include "dino.h"
#include "game.h"

#define MODE_OVERRIDE_FRAMES (60 * 5)

u32 dino_input = 0;
u32 coopFrames = 0;

void initCoop(STATE * state) {
    mmEffect(SFX_SCORE_REACHED);
    state->mode = COOP;

    DINO_STATE * mino = &state->minoState;
    mino->yPos = DINO_GROUND_Y - 70;
    mino->status = ENTERING;
    mino->jumpVelocity = INITIAL_JUMP_VELOCITY + 20;

    mino->xPos = 28;
    mino->hat = true;

    state->dinoState.status = RUNNING;
    state->dinoState.frameTime = RUN_FRAME;
    startGame(state);
}

void checkModeOverride(STATE * state) {
    if (key_is_down(KEY_L) && key_is_down(KEY_R)) {
        if (coopFrames++ >= MODE_OVERRIDE_FRAMES) {
            initCoop(state);
        }
    } else {
        coopFrames = 0;
    }
}

#define KEY_MAP(key, bit) \
dino_input |= key ? bit : 0

void inputP1(STATE * state) {
    dino_input = 0;
    if (state->mode == NORMAL) {
        KEY_MAP(key_hit(KEY_A | KEY_UP), BIT_HIT_UP);
        KEY_MAP(key_hit(KEY_DOWN), BIT_HIT_DOWN);
        KEY_MAP(key_released(KEY_A | KEY_UP), BIT_RELEASE_UP);
        KEY_MAP(key_released(KEY_DOWN), BIT_RELEASE_DOWN);
    } else if (state->mode == COOP) {
        KEY_MAP(key_hit(KEY_UP), BIT_HIT_UP);
        KEY_MAP(key_hit(KEY_DOWN), BIT_HIT_DOWN);
        KEY_MAP(key_released(KEY_UP), BIT_RELEASE_UP);
        KEY_MAP(key_released(KEY_DOWN), BIT_RELEASE_DOWN);
    }
}

void inputP2(STATE * state) {
    dino_input = 0;
    if (state->mode == COOP) {
        KEY_MAP(key_hit(KEY_A), BIT_HIT_UP);
        KEY_MAP(key_hit(KEY_B), BIT_HIT_DOWN);
        KEY_MAP(key_released(KEY_A), BIT_RELEASE_UP);
        KEY_MAP(key_released(KEY_B), BIT_RELEASE_DOWN);
    }
}