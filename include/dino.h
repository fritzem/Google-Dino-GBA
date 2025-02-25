#ifndef DINO_H
#define DINO_H

#include <stdlib.h>
#include "tonc.h"
#include "hitbox.h"
#include "obstacle.h"
#include "state.h"
#include "horizon.h"

#define SPEED 2946
//6000
#define SPEED_MAX 6383
//13000
#define ACCELERATION 1 
//0.001
#define SPEED_POINT 0x400
#define SPEED_POINT_DIV 10
#define SPEED_REM   0x3FF

#define DINO_GROUND_Y 7
#define INITIAL_JUMP_VELOCITY 100
#define DROP_VELOCITY -50
#define SPEED_DROP_COEFFICIENT 3
#define MAX_JUMP_HEIGHT 37
#define MIN_JUMP_HEIGHT 30 - DINO_GROUND_Y
#define GRAVITY -6
#define DINO_WIDTH 44
#define DINO_HEIGHT 47
#define DINO_WIDTH_DUCK 59
#define DINO_HEIGHT_DUCK 25
#define MAX_BLINKS 3
#define MAX_BLINK_DELAY 420
#define BLINK_TIME 30

#define CLEAR_FRAMES 180
#define STARTING_CURTAIN_SCROLL 468

#define RUN_FRAME 5
#define DUCK_FRAME 8

#define DINO_COLLISION_BOXES 6
#define DUCK_COLLISION_BOXES 1
#define CACT_COLLISION_BOXES 3
#define DACTYL_COLLISION_BOXES 5
#define MAX_HITBOXES 6



#define JUMP_HIT (key_hit(KEY_A) || key_hit(KEY_UP))
#define JUMP_RELEASED (key_released(KEY_A) || key_released(KEY_UP))
#define DINO_ANIMATING ((dinoState->status == RUNNING) || dinoState->status == DUCKING)

typedef enum {CRASHED, DUCKING, JUMPING, RUNNING, WAITING} DINO_STATUS;

typedef struct DINO_STATE {
	int xPos;
	int yPos;
	int jumpVelocity;

	DINO_STATUS status;
	bool speedDrop;
	bool jumped;
	bool reachedMin;

	int frame;
	int frameCounter;
	int frameTime;

	int blinkTime;
	int blinks;
	int blinkFrame;
	bool blinking;

	const int * animSI;
} DINO_STATE, DINO_STATE;

void updateDino(DINO_STATE * dinoState);
void inputDino(DINO_STATE * dinoState, GAME_STATE * gameState);

bool collisionCheck(DINO_STATE * dinoState, HORIZON_STATE * horizonState);

void addPoint(int add, int *base, int *point);

INLINE void initDino(DINO_STATE * dino) {
	dino->yPos = DINO_GROUND_Y;
	dino->status = WAITING;
}

void resetDino(DINO_STATE * dino);

#endif