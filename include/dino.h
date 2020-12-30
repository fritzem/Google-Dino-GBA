#ifndef DINO_H
#define DINO_H

#define INITIAL_JUMP_VELOCITY 100
#define DROP_VELOCITY -50
#define SPEED_DROP_COEFFICIENT 3
#define MIN_JUMP_HEIGHT 30
#define GRAVITY -6
#define STARTING_CURTAIN_SCROLL 468

#define RUN_FRAME 5
#define DUCK_FRAME 8

#define JUMP_HIT (key_hit(KEY_A) || key_hit(KEY_UP))
#define JUMP_RELEASED (key_released(KEY_A) || key_released(KEY_UP))
#define DINO_ANIMATING ((dinoState->status == RUNNING) || dinoState->status == DUCKING)

extern OBJ_ATTR obj_buffer[];
extern OBJ_AFFINE *obj_aff_buffer;

void update();

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



typedef struct GAME_STATE {
	int speed;
	int curtainScroll;

	bool playing;
	bool playingIntro;
} GAME_STATE, GAME_STATE;

extern GAME_STATE *gameState;

INLINE void initState(GAME_STATE * state) {
	state->speed = 0;
	state->playing = false;
	state->playingIntro = false;
	state->curtainScroll = STARTING_CURTAIN_SCROLL;
}

enum dinoStatus{CRASHED, DUCKING, JUMPING, RUNNING, WAITING};

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

#endif