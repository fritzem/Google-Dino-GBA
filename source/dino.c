#include "dino.h"
#include <maxmod.h>
#include <dino_soundbank.h>
#include "tonc.h"
#include "dinoSheetHelper.h"
#include "save.h"
#include "hitbox.h"
#include "game.h"
#include "obstacle.h"

const int animRun[] = {dinoFeet1_SI, dinoFeet2_SI};
const int animDuc[] = {dinoCrouchFeet0_SI, dinoCrouchFeet1_SI};

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

void updateDino() {
    if (dinoState->status == JUMPING)
        updateJump();
    if (dinoState->status == WAITING)
        updateBlink();
}

void input() {
	key_poll();
  	if (JUMP_HIT && (dinoState->status == WAITING || dinoState->status == RUNNING)) {
  			dinoJump();
  			return;
  	} else if (JUMP_RELEASED) {
  		if (dinoState->status == JUMPING)
  			endJump();
  		else if (dinoState->status == CRASHED && gameState->gameoverFrames >= RESET_FRAMES)
  			resetGame();
  		return; 
  	}

  	if (key_hit(KEY_DOWN)) {
  		if (dinoState->status == JUMPING) {
  			dinoState->speedDrop = true;
  			dinoState->jumpVelocity = -1;
  		} else if (dinoState->status == RUNNING) {
  			dinoDuck();
  		}
  	} else if (key_released(KEY_DOWN)) {
  		if (dinoState->status == JUMPING) {
  			dinoState->speedDrop = false;
  		} else if (dinoState->status == DUCKING) {
  			dinoRun();
  		}
  	}
}

void dinoJump() {
	if (!(gameState->playing))
	{
		hideTitle(titleSet);
		dinoUnBlink(dinoSet);
	}
	setDinoAnim(dinoSet, dinoFeet0_SI);

	dinoState->status = JUMPING;
	dinoState->speedDrop = false;
	dinoState->jumpVelocity = INITIAL_JUMP_VELOCITY + gameState->speed / SPEED_POINT / 10;
	dinoState->reachedMin = false;

	mmEffect(SFX_BUTTON_PRESSED);
}

void updateJump() {
	if (dinoState->speedDrop)
		dinoState->yPos += (dinoState->jumpVelocity * SPEED_DROP_COEFFICIENT) / 10;
	else
		dinoState->yPos += (dinoState->jumpVelocity) / 10;

	dinoState->jumpVelocity += GRAVITY;

	if (dinoState->yPos > MIN_JUMP_HEIGHT || dinoState->speedDrop)
		dinoState->reachedMin = true;
	if (dinoState->yPos > MAX_JUMP_HEIGHT || dinoState->speedDrop)
		endJump();

	if (dinoState->yPos < DINO_GROUND_Y) {
		bool drop = dinoState->speedDrop;
		dinoRun();
		if (drop)
			dinoDuck();

		//if first landing, pull back the curtain
		if (!dinoState->jumped)
		{
			gameState->playingIntro = true;
			gameState->playing = true;
			dinoState->jumped = true;
			sqran(gameState->randoFrames);
			addCloud();
		}
	}

}

void endJump() {
	if (dinoState->reachedMin && dinoState->jumpVelocity < DROP_VELOCITY)
		dinoState->jumpVelocity = DROP_VELOCITY;
}

void updateBlink() {
	if (!(dinoState->blinking)) {
		if (dinoState->blinkFrame == 0) {
			if (dinoState->blinks >= MAX_BLINKS)
				return;
			dinoState->blinkTime = getBlinkTime();
		}
		dinoState->blinkFrame += 1;
		if (dinoState->blinkFrame >= dinoState->blinkTime) {
			dinoState->blinking = true;
			dinoBlink(dinoSet);
			dinoState->blinkFrame = 0;
		}
	} else {
		dinoState->blinkFrame += 1;
		if (dinoState->blinkFrame >= BLINK_TIME) {
			dinoState->blinking = false;
			dinoState->blinkFrame = 0;
			dinoUnBlink(dinoSet);
			dinoState->blinks += 1;
		}
	}
}

int getBlinkTime() {
	return qran_range(0, MAX_BLINK_DELAY);
}

void dinoRun() {
	dinoState->frame = 0;
	dinoState->frameCounter = 0;
	dinoState->frameTime = RUN_FRAME;
	dinoState->animSI = animRun;

	dinoState->status = RUNNING;
  	setDinoUpright(dinoSet);
  	dinoState->speedDrop = false;

 	dinoState->jumpVelocity = 0;
	dinoState->yPos = DINO_GROUND_Y;
}

void dinoDuck() {
	dinoState->frame = 0;
	dinoState->frameCounter = 0;
	dinoState->frameTime = DUCK_FRAME;
	dinoState->animSI = animDuc;

	dinoState->status = DUCKING;
  setDinoDucking(dinoSet);
}

void gameOver() {
	mmEffect(SFX_HIT);

	dinoState->status = CRASHED;
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

void resetDino(DINO_STATE * dino) {
	dino->xPos = 0;
	dino->yPos = DINO_GROUND_Y;
	dino->jumpVelocity = 0;

	dino->status = RUNNING;
	dino->speedDrop = false;
	dino->jumped = true;
	dino->reachedMin = false;

	dino->frame = 0;
	dino->frameCounter = 0;
	dino->frameTime = RUN_FRAME;
	dinoState->animSI = animRun;
}

void addPoint(int add, int *base, int *point) {
	*base += (add >> SPEED_POINT_DIV);
	*point += (add & SPEED_REM);
	if (*point >> SPEED_POINT_DIV) {
		*base += (*point >> SPEED_POINT_DIV);
		*point = *point & SPEED_REM;
	}
}

bool collisionCheck() {
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


