#include <string.h>
#include "tonc.h"
#include "dino.h"
#include "dinoSheet.h"
#include "dinoSheetHelper.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

GAME_STATE *gameState;
DINO_STATE *dinoState;
HORIZON_STATE *horizonState;

const int animRun[] = {dinoFeet1_SI, dinoFeet2_SI};
const int animDuc[] = {dinoCrouchFeet0_SI, dinoCrouchFeet1_SI};

int main()
{	
	init();

  while(1) {
  	vid_vsync();
  	
    input();
    update();


  	oam_copy(oam_mem, obj_buffer, 64);
  }
  return 0;
}

void update() {

	if (dinoState->status == JUMPING)
			updateJump();
	if (gameState->playing) {

		//If 3 second grace period expires, spawn obstacles
		//Accounts for overflow if you somehow make it 400 or so days
		gameState->runningFrames += 1;
		gameState->spawnObstacles = (gameState->runningFrames > CLEAR_FRAMES) 
				| gameState->spawnObstacles; 

		if (gameState->playingIntro) {
			gameState->curtainScroll -= 8;
			if (gameState->curtainScroll <= 256) {
				gameState->curtainScroll = 256;
				gameState->playingIntro = false;
			}
			REG_BG1HOFS = gameState->curtainScroll;
		} else {
			updateHorizon();
		}

		bool collision = false; //didDinoHitSomethingUhhh();

		if (!collision) {
			addPoint(gameState->speed, &gameState->distanceRan, &gameState->distanceRanPoint);

			if (gameState->speed < SPEED_MAX)
				gameState->speed += ACCELERATION;
		}

		updateDistanceMeter((gameState->distanceRan) + ((gameState->distanceRanPoint) ? 1 : 0));

		//updateGraphics()??
		dinoState->frameCounter += 1;
		if ((DINO_ANIMATING) && (dinoState->frameCounter >= dinoState->frameTime)) {
			dinoState->frame = dinoState->frame == 1 ? 0 : 1;
			dinoState->frameCounter = 0;
		}
		if (DINO_ANIMATING) setDinoAnim(dinoSet, dinoState->animSI[dinoState->frame]);
	} else {
		gameState->randoFrames += 1;
	}


	
	dinoGraphicsUpdate(dinoSet);
}

void updateHorizon() {
	int scrolled = gameState->speed / SPEED_POINT;
	horizonState->scroll += scrolled;
	horizonState->scrolled += scrolled;
	
	//horizon terrain
	if (horizonState->scrolled >= TILE_SIZE) {

		horizonState->terrainScroll += 1;
		if (horizonState->terrainScroll / TERRAIN_STRIP_LENGTH) horizonState->bumpy = randomBool();
		horizonState->terrainScroll %= TERRAIN_STRIP_LENGTH;

		updateHorizonTile(horizonState->nextScrollTile, horizonState->terrainScroll, horizonState->bumpy);

		horizonState->scrolled %= TILE_SIZE;
		horizonState->nextScrollTile += 1;
		horizonState->nextScrollTile %= BG_TILE_LENGTH;
	}

	//clouds
	if (horizonState->cloudCount) {
		int tempCursor = horizonState->cloudCursor;
		int lastCursor = 0;
		int preCount = horizonState->cloudCount;
		for (int i = 0; i < preCount; i++) {
			lastCursor = tempCursor;
			updateCloud((horizonState->clouds) + tempCursor);
			tempCursor = (tempCursor + 1) % MAX_CLOUDS;
		}

		if ((horizonState->cloudCount < MAX_CLOUDS && 
				((SCREEN_WIDTH - ((horizonState->clouds + lastCursor)->xPos)) > 
				(horizonState->clouds + lastCursor)->cloudGap)) && randomBool()) {
			addCloud();
		}
	} else {
		addCloud();
	}


	REG_BG0HOFS = horizonState->scroll;
}

void updateDistanceMeter(int distance) {
	setNumValue(scoreSet, distanceConvert(distance));
}

void addCloud() {
	int newCursor = (horizonState->cloudCursor + horizonState->cloudCount) % MAX_CLOUDS;
	horizonState->cloudCount += 1;
	

	initCloud(((horizonState->clouds) + newCursor), newCursor);
}

void updateCloud(CLOUD * cloud) {
	cloud->xPos -= 1;
	setCloudPos((clouds + (cloud->cloudNum)), cloud->xPos, cloud->yPos);
	if (!cloudVisible(cloud)) {
		horizonState->cloudCursor = (horizonState->cloudCursor + 1) % MAX_CLOUDS;
		horizonState->cloudCount -= 1;
	}
}

bool cloudVisible(CLOUD * cloud) {
	return ((cloud->xPos) > -CLOUD_WIDTH);
}

int distanceConvert(int distance) {
	return distance / 40; // (* 0.025)
}

void input() {
	key_poll();
  	if (JUMP_HIT && (dinoState->status == WAITING || dinoState->status == RUNNING)) {
  			dinoJump();
  			return;
  	} else if (JUMP_RELEASED) {
  		if (dinoState->status == JUMPING)
  			endJump();
  		else if (dinoState->status == CRASHED)
  			//resetGame();
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
  		} else if (dinoState->status == DUCKING){
  			dinoRun();
  		}
  	}
}

void dinoJump() {
	setDinoAnim(dinoSet, dinoFeet0_SI);

	dinoState->status = JUMPING;
	dinoState->speedDrop = false;
	dinoState->jumpVelocity = INITIAL_JUMP_VELOCITY; // + speed/10 ??
}

void updateJump() {
	if (dinoState->speedDrop)
		dinoState->yPos += (dinoState->jumpVelocity * SPEED_DROP_COEFFICIENT) / 10;
	else
		dinoState->yPos += (dinoState->jumpVelocity) / 10;

	dinoState->jumpVelocity += GRAVITY;

	//Min height
	if (dinoState->yPos > MIN_JUMP_HEIGHT || dinoState->speedDrop)
		endJump();

	if (dinoState->yPos < 0) {
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
	if (dinoState->jumpVelocity < DROP_VELOCITY)
		dinoState->jumpVelocity = DROP_VELOCITY;
}

void resetDino() {
	dinoState->status = RUNNING;
	dinoState->speedDrop = false;

	dinoState->jumpVelocity = 0;
	dinoState->yPos = 0;
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
	dinoState->yPos = 0;
}

void dinoDuck() {
	dinoState->frame = 0;
	dinoState->frameCounter = 0;
	dinoState->frameTime = DUCK_FRAME;
	dinoState->animSI = animDuc;

	dinoState->status = DUCKING;
  setDinoDucking(dinoSet);
}

void init() {
	initMem();
	initGraphics();
	initGame();
}

void initMem() {
	memcpy(&tile_mem[4][0], dinoSheetTiles, dinoSheetTilesLen);
	memcpy(pal_obj_mem, dinoSheetPal, dinoSheetPalLen);
	memcpy(pal_bg_mem, dinoSheetPal, dinoSheetPalLen);
	memcpy(&tile_mem[0][0], dinoSheetTiles, dinoSheetTilesLen);
}

void initGraphics() {
	whiteOutBG();
	backgroundInit();

  //horizon layer
  REG_BG0CNT = BG_PRIO(1) | BG_CBB(0) | BG_SBB(31) | BG_4BPP | BG_REG_32x32;
  //curtain layer
  REG_BG1CNT = BG_PRIO(0) | BG_CBB(0) | BG_SBB(29) | BG_4BPP | BG_REG_64x32;

	oam_init(obj_buffer, 128);
	initSets();
	assembleSets();

  REG_DISPCNT= DCNT_OBJ | DCNT_OBJ_2D | DCNT_BG0 | DCNT_BG1;
}

void initGame() {
	gameState = malloc(sizeof(GAME_STATE));
	initState(gameState);
	dinoState = malloc(sizeof(DINO_STATE));
	initDino(dinoState);
	horizonState = malloc(sizeof(HORIZON_STATE));
	initHorizon(horizonState);
}

void addPoint(int add, int *base, int *point) {
	*base += add / SPEED_POINT;
	*point += add % SPEED_POINT;
	if (*point / SPEED_POINT) {
		*base += *point / SPEED_POINT;
		*point = *point % SPEED_POINT;
	}
}

bool randomBool() {
	return ((qran_range(0, 1000) > 500) ? true : false);
}

//bitmap display
void bitDisp()
{
    REG_DISPCNT= DCNT_MODE3 | DCNT_BG2;

    m3_mem[80][120]= CLR_RED;
    m3_mem[80][136]= CLR_LIME;
    m3_mem[96][120]= CLR_BLUE;

    while(1) {
    	vid_vsync();
    	m3_mem[80][120]= CLR_BLUE;

    	key_poll();
    	if (key_is_down(KEY_A))
    	{
    		m3_mem[80][120]= CLR_RED;
    	}
    }
}