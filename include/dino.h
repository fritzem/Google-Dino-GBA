#ifndef DINO_H
#define DINO_H

#include <stdlib.h>
#include "tonc.h"

#define SPEED 2880
//6000
#define SPEED_MAX 6240
//13000
#define ACCELERATION 1 
//0.001
#define SPEED_POINT 1000

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

#define INITIAL_JUMP_VELOCITY 100
#define DROP_VELOCITY -50
#define SPEED_DROP_COEFFICIENT 3
#define MIN_JUMP_HEIGHT 30
#define GRAVITY -6

#define CLEAR_FRAMES 180

#define STARTING_CURTAIN_SCROLL 468

#define RUN_FRAME 5
#define DUCK_FRAME 8


#define JUMP_HIT (key_hit(KEY_A) || key_hit(KEY_UP))
#define JUMP_RELEASED (key_released(KEY_A) || key_released(KEY_UP))
#define DINO_ANIMATING ((dinoState->status == RUNNING) || dinoState->status == DUCKING)

extern OBJ_ATTR obj_buffer[];
extern OBJ_AFFINE *obj_aff_buffer;

void update();

void updateNight();
void updateHorizon();
void placeStars();
void updateDistanceMeter(int distance);

void input();
void dinoJump();
void updateJump();
void endJump();
void resetDino();
void dinoRun();
void dinoDuck();


void init();
void initMem();
void initGraphics();
void initGame();

void addPoint(int add, int *base, int *point);
int distanceConvert(int distance);
bool randomBool();


typedef struct GAME_STATE {
	int speed;
	int curtainScroll;

	int distanceRan;
	int distanceRanPoint;

	int runningFrames;
	int randoFrames;
	bool spawnObstacles;

	bool playing;
	bool playingIntro;
} GAME_STATE, GAME_STATE;

extern GAME_STATE *gameState;

INLINE void initState(GAME_STATE * state) {
	state->speed = SPEED;
	state->curtainScroll = STARTING_CURTAIN_SCROLL;
	state->distanceRan = 0;
	state->distanceRanPoint = 0;

	state->runningFrames = 0;
	state->randoFrames = 0;
	state->spawnObstacles = false;

	state->playing = false;
	state->playingIntro = false;
	
}

enum dinoStatus{CRASHED, DUCKING, JUMPING, RUNNING, WAITING};

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

void addCloud();
void updateCloud(CLOUD * cloud);
bool cloudVisible(CLOUD * cloud);

typedef struct HORIZON_STATE {
	int scroll;
	int nextScrollTile;
	int scrolled;
	int terrainScroll;
	bool bumpy;

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
} HORIZON_STATE, HORIZON_STATE;

extern HORIZON_STATE *horizonState;

INLINE void initHorizon(HORIZON_STATE * horizon) {
	horizon->scroll = 0;
	horizon->nextScrollTile = 31;
	horizon->scrolled = 0;
	horizon->terrainScroll = 31;
	horizon->bumpy = false;

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
}

typedef struct DINO_STATE {
	int xPos;
	int yPos;
	int jumpVelocity;

	enum dinoStatus status;
	bool speedDrop;
	bool jumped;

	int frame;
	int frameCounter;
	int frameTime;
	const int * animSI;
} DINO_STATE, DINO_STATE;

extern DINO_STATE *dinoState;

INLINE void initDino(DINO_STATE * dino) {
	dino->xPos = 0;
	dino->yPos = 0;
	dino->jumpVelocity = 0;

	dino->status = WAITING;
	dino->speedDrop = false;
	dino->jumped = false;

	dino->frame = 0;
	dino->frameCounter = 0;
	dino->frameTime = 0;
}

typedef struct METER_STATE {
	int distance;
	int invertCounter;

} METER_STATE, METER_STATE;

INLINE void initMeter(METER_STATE * meter) {
	meter->distance = 0;
	meter->invertCounter = 0;
}

#endif