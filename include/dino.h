#ifndef DINO_H
#define DINO_H

#include <stdlib.h>
#include "tonc.h"

#define SPEED 2946
//6000
#define SPEED_MAX 6383
//13000
#define ACCELERATION 1 
//0.001
#define SPEED_POINT 0x400
#define SPEED_POINT_DIV 10
#define SPEED_REM   0x3FF

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

#define OBSTACLE_TYPES 3
#define MAX_OBSTACLES 2
#define MAX_OBSTACLE_SIZE 3
#define CACTUS_SMALL 0
#define CACTUS_LARGE 1
#define PTERODACTYL  2
#define CACTUS_SMALL_Y 113
#define CACTUS_SMALL_Y_SPRITE_OFFSET 5
#define CACTUS_SMALL_WIDTH 17
#define CACTUS_SMALL_HEIGHT 35
#define CACTUS_SMALL_MULTI_SPEED 4
#define CACTUS_LARGE_Y 103
#define CACTUS_LARGE_WIDTH 25
#define CACTUS_LARGE_HEIGHT 50
#define CACTUS_LARGE_MULTI_SPEED 6
#define CACTUS_GAP 120 * SPEED_POINT
#define DACTYL_WIDTH 46
#define DACTYL_HEIGHT 40
#define DACTYL_GAP 150 * SPEED_POINT
#define DACTYL_FRAMES 10
#define DACTYL_MIN_SPEED 6000
#define DACTYL_SPEED_OFFSET 800

#define INVERT_DISTANCE 700
#define INVERT_FRAMES 90
#define INVERT_FADE_DURATION 720

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

#define ACHIEVEMENT_DISTANCE 100
#define FLASH_FRAMES 15
#define FLASH_ITERATIONS 3

#define CLEAR_FRAMES 180

#define STARTING_CURTAIN_SCROLL 468

#define RUN_FRAME 5
#define DUCK_FRAME 8

#define DINO_COLLISION_BOXES 6
#define DUCK_COLLISION_BOXES 1
#define CACT_COLLISION_BOXES 3
#define DACTYL_COLLISION_BOXES 5
#define MAX_HITBOXES 6

#define RESET_FRAMES 45

#define HISCORE_SRAM 0x0E000000

#define JUMP_HIT (key_hit(KEY_A) || key_hit(KEY_UP))
#define JUMP_RELEASED (key_released(KEY_A) || key_released(KEY_UP))
#define DINO_ANIMATING ((dinoState->status == RUNNING) || dinoState->status == DUCKING)

extern OBJ_ATTR obj_buffer[];
extern OBJ_AFFINE *obj_aff_buffer;

extern u8 * hiScore;

void update();

void updateNight();
void updateHorizon();
void placeStars();
bool updateDistanceMeter(int distance);
void updateObstacles(int scrollSpeed);
void addObstacle();

void input();
void dinoJump();
void updateJump();
void endJump();
void updateBlink();
int getBlinkTime();
void dinoRun();
void dinoDuck();

void gameOver();

void init();
void initSound();
void initMem();
void initGraphics();
void initGame();
void resetGame();

void addPoint(int add, int *base, int *point);
int distanceConvert(int distance);
bool randomBool();
int readHiscore();
void setHiscore(int score);

typedef struct COLLISION_BOX {
	int x;
	int y;
	int w;
	int h;
} COLLISION_BOX, COLLISION_BOX;

void adjustBox(COLLISION_BOX * box, int size, int width);
void cloneBox(COLLISION_BOX * clone, const COLLISION_BOX * base, int boxes);
bool collisionCheck();
bool boxCheck(COLLISION_BOX * a, COLLISION_BOX * b);
bool boxCheckOffset(const COLLISION_BOX * a, COLLISION_BOX * b,
							int x1, int y1, int x2, int y2);

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

	int gameoverFrames;
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
	
	state->gameoverFrames = 0;
}

INLINE void resetState(GAME_STATE * state) {
	state->speed = SPEED;
	state->distanceRan = 0;
	state->distanceRanPoint = 0;

	state->runningFrames = 0;
	state->spawnObstacles = false;

	state->playing = true;
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

typedef struct OBSTACLE {
	int type;
	int x;
	int y;
	int size;
	int width;
	int height;
	int gap;
	int speedOffset;
	bool visible;

	int frames;
	int extraSpeed;

	int numBoxes;
	COLLISION_BOX * colBox;
	int spriteY;
} OBSTACLE, OBSTACLE;

INLINE void resetObstacles(OBSTACLE * obs) {
	obs->visible = 0;
	(obs + 1)->visible = 0;
}

void createCactusSmall(OBSTACLE * obs);
void createCactusLarge(OBSTACLE * obs);
void createPterodactyl(OBSTACLE * obs);
void updateObstacle(OBSTACLE * obs, int scrollSpeed, int index);

typedef struct HORIZON_STATE {
	int scroll;
	int nextScrollTile;
	int scrolled;
	int terrainScroll;
	bool bumpy;
	int extraScroll;

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

	OBSTACLE * obstacles;
	int obstacleCount;
	int obstacleCursor;
	int lastObstacle;
} HORIZON_STATE, HORIZON_STATE;

extern HORIZON_STATE *horizonState;

INLINE void initHorizon(HORIZON_STATE * horizon) {
	horizon->scroll = 0;
	horizon->nextScrollTile = 31;
	horizon->scrolled = 0;
	horizon->terrainScroll = 31;
	horizon->bumpy = false;
	horizon->extraScroll = 0;

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

	horizon->obstacles = malloc(MAX_OBSTACLES * sizeof(OBSTACLE));
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		(horizon->obstacles + i)->colBox = malloc(MAX_HITBOXES * sizeof(COLLISION_BOX));
	}

	horizon->obstacleCount = 0;
	horizon->obstacleCursor = 0;
	horizon->lastObstacle = 0;
}

INLINE void resetHorizon(HORIZON_STATE * horizon) {
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

typedef struct DINO_STATE {
	int xPos;
	int yPos;
	int jumpVelocity;

	enum dinoStatus status;
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

extern DINO_STATE *dinoState;

INLINE void initDino(DINO_STATE * dino) {
	dino->xPos = 0;
	dino->yPos = DINO_GROUND_Y;
	dino->jumpVelocity = 0;

	dino->status = WAITING;
	dino->speedDrop = false;
	dino->jumped = false;
	dino->reachedMin = false;

	dino->frame = 0;
	dino->frameCounter = 0;
	dino->frameTime = 0;

	dino->blinkTime = 0;
	dino->blinks = 0;
	dino->blinkFrame = 0;
	dino->blinking = false;
}

void resetDino(DINO_STATE * dino);

typedef struct METER_STATE {
	int distance;
	int invertCounter;
	int achievementCounter;
	int flashFrame;
	int flashIteration;
	bool achieving;
} METER_STATE, METER_STATE;

INLINE void initMeter(METER_STATE * meter) {
	meter->distance = 0;
	meter->invertCounter = 0;
	meter->achievementCounter = 0;
	meter->achieving = false;
	meter->flashFrame = 0;
	meter->flashIteration = 0;
}

#endif