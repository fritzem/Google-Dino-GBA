#include "dino.h"
#include <maxmod.h>
#include <dino_soundbank.h>
#include "tonc.h"
#include "hitbox.h"
#include "obstacle.h"
#include "input.h"

const COLLISION_BOX dinoBoxes[] = {
	{22, 0, 17, 16},
	{1, 18, 30, 9},
	{10, 35, 14, 8},
	{1, 24, 29, 5},
	{5, 30, 21, 4},
	{9, 34, 15, 4}
};

const COLLISION_BOX duckBoxes[] = {
	{1, 18, 55, 25}	
};

void dinoJump(DINO_STATE * dinoState, GAME_STATE * gameState);
void updateJump(DINO_STATE * dinoState);
void updateEntry(DINO_STATE * dinoState);
void endJump(DINO_STATE * dinoState);
void updateBlink(DINO_STATE * dinoState);
void dinoRun(DINO_STATE * dinoState);
void dinoDuck(DINO_STATE * dinoState);

int getBlinkTime();

void updateDino(DINO_STATE * dinoState) {
    if (dinoState->status == JUMPING)
        updateJump(dinoState);
    if (dinoState->status == WAITING)
        updateBlink(dinoState);
    if (dinoState->status == ENTERING)
        updateEntry(dinoState);

    dinoState->frameCounter += 1;
    if ((DINO_ANIMATING) && (dinoState->frameCounter >= dinoState->frameTime)) {
        dinoState->frame = dinoState->frame == 1 ? 0 : 1;
        dinoState->frameCounter = 0;
    }
}

void inputDino(DINO_STATE * dinoState, GAME_STATE * gameState) {
    if (dinoState->status == CRASHED || dinoState->status == ENTERING) {
        return;
    }

  	if (HIT_UP && (dinoState->status == WAITING || dinoState->status == RUNNING)) {
        dinoJump(dinoState, gameState);
        return;
  	} else if (RELEASE_UP) {
  		if (dinoState->status == JUMPING)
  			endJump(dinoState);
  	}

  	if (HIT_DOWN) {
  		if (dinoState->status == JUMPING) {
  			dinoState->speedDrop = true;
  			dinoState->jumpVelocity = -1;
  		} else if (dinoState->status == RUNNING) {
  			dinoDuck(dinoState);
  		}
  	} else if (RELEASE_DOWN) {
  		if (dinoState->status == JUMPING) {
  			dinoState->speedDrop = false;
  		} else if (dinoState->status == DUCKING) {
  			dinoRun(dinoState);
  		}
  	}
}

void dinoJump(DINO_STATE * dinoState, GAME_STATE * gameState) {
	dinoState->status = JUMPING;
	dinoState->speedDrop = false;
	dinoState->jumpVelocity = INITIAL_JUMP_VELOCITY + gameState->speed / SPEED_POINT / 10;
	dinoState->reachedMin = false;
    dinoState->blinking = false;
	mmEffect(SFX_BUTTON_PRESSED);
}

void updateJump(DINO_STATE * dinoState) {
	if (dinoState->speedDrop)
		dinoState->yPos += (dinoState->jumpVelocity * SPEED_DROP_COEFFICIENT) / 10;
	else
		dinoState->yPos += (dinoState->jumpVelocity) / 10;

	dinoState->jumpVelocity += GRAVITY;

	if (dinoState->yPos > MIN_JUMP_HEIGHT || dinoState->speedDrop)
		dinoState->reachedMin = true;
	if (dinoState->yPos > MAX_JUMP_HEIGHT || dinoState->speedDrop)
		endJump(dinoState);

	if (dinoState->yPos < DINO_GROUND_Y) {
		bool drop = dinoState->speedDrop;
		dinoRun(dinoState);
		if (drop)
			dinoDuck(dinoState);

		//if first landing, pull back the curtain
        dinoState->jumped |= !dinoState->jumped;
	}
}

void updateEntry(DINO_STATE * dinoState) {
    dinoState->yPos += (dinoState->jumpVelocity) / 10;
    dinoState->jumpVelocity += GRAVITY;

    if (dinoState->yPos > MIN_JUMP_HEIGHT || dinoState->speedDrop)
        dinoState->reachedMin = true;
    if (dinoState->yPos > MAX_JUMP_HEIGHT || dinoState->speedDrop)
        endJump(dinoState);

    if (dinoState->yPos < DINO_GROUND_Y && dinoState->reachedMin) {
        bool drop = dinoState->speedDrop;
        dinoRun(dinoState);
        if (drop)
            dinoDuck(dinoState);

        //if first landing, pull back the curtain
        dinoState->jumped |= !dinoState->jumped;
    }
}

void endJump(DINO_STATE * dinoState) {
	if (dinoState->reachedMin && dinoState->jumpVelocity < DROP_VELOCITY)
		dinoState->jumpVelocity = DROP_VELOCITY;
}

void updateBlink(DINO_STATE * dinoState) {
	if (!(dinoState->blinking)) {
		if (dinoState->blinkFrame == 0) {
			if (dinoState->blinks >= MAX_BLINKS)
				return;
			dinoState->blinkTime = getBlinkTime();
		}
		dinoState->blinkFrame += 1;
		if (dinoState->blinkFrame >= dinoState->blinkTime) {
			dinoState->blinking = true;
			dinoState->blinkFrame = 0;
		}
	} else {
		dinoState->blinkFrame += 1;
		if (dinoState->blinkFrame >= BLINK_TIME) {
			dinoState->blinking = false;
			dinoState->blinkFrame = 0;
			dinoState->blinks += 1;
		}
	}
}

int getBlinkTime() {
	return qran_range(0, MAX_BLINK_DELAY);
}

void dinoRun(DINO_STATE * dinoState) {
	dinoState->frame = 0;
	dinoState->frameCounter = 0;
	dinoState->frameTime = RUN_FRAME;

	dinoState->status = RUNNING;
  	dinoState->speedDrop = false;

 	dinoState->jumpVelocity = 0;
	dinoState->yPos = DINO_GROUND_Y;
}

void dinoDuck(DINO_STATE * dinoState) {
	dinoState->frame = 0;
	dinoState->frameCounter = 0;
	dinoState->frameTime = DUCK_FRAME;

	dinoState->status = DUCKING;
}

void resetDino(DINO_STATE * dino) {
	dino->yPos = DINO_GROUND_Y;
	dino->jumpVelocity = 0;

	dino->status = RUNNING;
	dino->speedDrop = false;
	dino->jumped = true;
	dino->reachedMin = false;

	dino->frame = 0;
	dino->frameCounter = 0;
	dino->frameTime = RUN_FRAME;
}

void addPoint(int add, int *base, int *point) {
	*base += (add >> SPEED_POINT_DIV);
	*point += (add & SPEED_REM);
	if (*point >> SPEED_POINT_DIV) {
		*base += (*point >> SPEED_POINT_DIV);
		*point = *point & SPEED_REM;
	}
}

bool collisionCheck(DINO_STATE * dinoState, HORIZON_STATE * horizonState) {
    int tW = ((dinoState->status == DUCKING) ? DINO_WIDTH_DUCK : DINO_WIDTH) - 2;
    int tH = ((dinoState->status == DUCKING) ? DINO_HEIGHT_DUCK : DINO_HEIGHT) - 2;
    int tX = dinoState->xPos + 1;
    int tY = SCREEN_HEIGHT - dinoState->yPos - tH - 1;

    COLLISION_BOX dBox = {tX, tY, tW, tH};
    COLLISION_BOX oBox = {0,0,0,0};

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        OBSTACLE *obs = (horizonState->obstacles + i);
        if (obs->visible) {
            oBox.w = obs->width - 2;
            oBox.h = obs->height - 2;
            oBox.x = obs->x + 1;
            oBox.y = obs->spriteY + 1;

            if (boxCheck(&dBox, &oBox))
            {
                if (dinoState->status == DUCKING) {
                    for (int k = 0; k < obs->numBoxes; k++) {
                        if (boxCheckOffset(duckBoxes, (obs->colBox + k),
                                           tX, tY, oBox.x, oBox.y)) {
                            return true;
                        }
                    }
                } else {
                    for (int i = 0; i < DINO_COLLISION_BOXES; i++) {
                        for (int k = 0; k < obs->numBoxes; k++) {
                            if (boxCheckOffset(dinoBoxes + i, (obs->colBox + k),
                                               tX, tY, oBox.x, oBox.y)) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}


