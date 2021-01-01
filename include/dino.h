#ifndef DINO_H
#define DINO_H

#define SPEED 2880
//6000
#define SPEED_MAX 6240
//13000
#define ACCELERATION 1 
//0.001
#define SPEED_POINT 1000

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

void updateHorizon();
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



typedef struct GAME_STATE {
	int speed;
	int curtainScroll;

	int distanceRan;
	int distanceRanPoint;

	int runningFrames;
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
	state->spawnObstacles = false;

	state->playing = false;
	state->playingIntro = false;
	
}

enum dinoStatus{CRASHED, DUCKING, JUMPING, RUNNING, WAITING};

typedef struct HORIZON_STATE {
	int scroll;
} HORIZON_STATE, HORIZON_STATE;

extern HORIZON_STATE *horizonState;

INLINE void initHorizon(HORIZON_STATE * horizon) {
	horizon->scroll = 0;
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

} METER_STATE, METER_STATE;

#endif