#ifndef DINO_H
#define DINO_H

#define DROP_VELOCITY -5
#define GRAVITY 6

typedef struct GAME_STATE {

} GAME_STATE, GAME_STATE;

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

void initDino(DINO_STATE * dino) {
	dino->xPos = 0;
	dino->yPos = 0;
	dino->jumpVelocity = 0;

	dino->status = WAITING;
	dino->jumping = false;
	dino->ducking = false;
	dino->speedDrop = false;
}

#endif