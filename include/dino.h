#ifndef DINO_H
#define DINO_H

#define DROP_VELOCITY -5
#define GRAVITY 6

extern OBJ_ATTR obj_buffer[];
extern OBJ_AFFINE *obj_aff_buffer;

void init();
void initMem();
void initGraphics();
void initGame();



typedef struct GAME_STATE {

} GAME_STATE, GAME_STATE;

extern GAME_STATE *gameState;

enum dinoStatus{CRASHED, DUCKING, JUMPING, RUNNING, WAITING};

typedef struct DINO_STATE {
	int xPos;
	int yPos;
	int jumpVelocity;

	enum dinoStatus status;
	bool jumping;
	bool ducking;
	bool speedDrop;
} DINO_STATE, DINO_STATE;

extern DINO_STATE *dinoState;

INLINE void initDino(DINO_STATE * dino) {
	dino->xPos = 0;
	dino->yPos = 0;
	dino->jumpVelocity = 0;

	dino->status = WAITING;
	dino->jumping = false;
	dino->ducking = false;
	dino->speedDrop = false;
}

#endif