#include <string.h>
#include <maxmod.h>
#include "tonc.h"
#include "dino.h"
#include "soundbank.h"
#include "soundbank_bin.h"
#include "dinoSheet.h"
#include "dinoSheetHelper.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

GAME_STATE *gameState;
DINO_STATE *dinoState;
HORIZON_STATE *horizonState;
METER_STATE *meterState;

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

const COLLISION_BOX cactSmallBoxes[] = {
	{0, 7, 5, 27},
	{4, 0, 6, 34},
	{10, 4, 7, 14}
};

const COLLISION_BOX cactLargeBoxes[] = {
	{0, 12, 7, 38},
	{8, 0, 7, 49},
	{13, 10, 10, 38}
};

const COLLISION_BOX pterodactylBoxes[] = {
	{15, 15, 16, 5},
	{18, 21, 24, 6},
	{2, 14, 4, 3},
	{6, 10, 4, 7},
	{10, 8, 6, 9}
};

int main()
{	
	init();

  	while(1) {
		mmFrame();
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
				|| gameState->spawnObstacles; 

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

		bool collision = gameState->spawnObstacles &&
			collisionCheck();

		if (!collision) {
			addPoint(gameState->speed, &gameState->distanceRan, &gameState->distanceRanPoint);

			if (gameState->speed < SPEED_MAX)
				gameState->speed += ACCELERATION;
		} else {
			gameOver();
		}

		if (updateDistanceMeter((gameState->distanceRan) + ((gameState->distanceRanPoint) ? 1 : 0)))
			mmEffect(SFX_SCORE_REACHED);

		updateNight();

		dinoState->frameCounter += 1;
		if ((DINO_ANIMATING) && (dinoState->frameCounter >= dinoState->frameTime)) {
			dinoState->frame = dinoState->frame == 1 ? 0 : 1;
			dinoState->frameCounter = 0;
		}
		if (DINO_ANIMATING) setDinoAnim(dinoSet, dinoState->animSI[dinoState->frame]);
	} else {
		gameState->randoFrames += 1;
		gameState->gameoverFrames += 1;
	}


	
	dinoGraphicsUpdate(dinoSet);
}

void updateHorizon() {
	int scrollSpeed = (gameState->speed + horizonState->extraScroll);
	int scrolled =  scrollSpeed / SPEED_POINT;
	horizonState->extraScroll = (gameState->speed + horizonState->extraScroll) % SPEED_POINT;
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

	//night
	if (horizonState->inverting) {
		horizonState->invertFrame += 1;
		bool invertOver = horizonState->invertFrame >= INVERT_FRAMES;
		inversionUpdate(horizonState->night, invertOver);

		if (invertOver) {
			horizonState->inverting = false;
			horizonState->invertFrame = 0;
		}
	}

	if (horizonState->fading) {
		int truOpac = horizonState->opacity / 2;
		REG_BLDALPHA = BLDA_BUILD(truOpac, 16 - truOpac);
 		REG_BLDY= BLDY_BUILD(16 - truOpac);

 		if (horizonState->night)
 			horizonState->opacity += 1;
 		else
 			horizonState->opacity -= 1;

 		if (horizonState->opacity == 32 || horizonState->opacity == 0)
			horizonState->fading = false;
	}

	if (horizonState->night || horizonState->fading) {
		horizonState->starMov += STAR_SPEED;
		horizonState->moonMov += MOON_SPEED;

		if (horizonState->starMov >= STAR_MOVE_THRESHOLD) {
			horizonState->starMov -= STAR_MOVE_THRESHOLD;
			horizonState->star0X -= 1;
			horizonState->star1X -= 1;
			setStarPos(starsSet,
				horizonState->star0X,
				horizonState->star0Y,
				horizonState->star1X,
				horizonState->star1Y
			);
		} else {
			if (horizonState->star0X < -SCREEN_WIDTH)
				horizonState->star0X = SCREEN_WIDTH;
			if (horizonState->star1X < -SCREEN_WIDTH)
				horizonState->star1X = SCREEN_WIDTH;
		}

		if (horizonState->moonMov >= MOON_MOVE_THRESHOLD) {
			horizonState->moonX -= 1;
			horizonState->moonMov = 0;
			setMoonPos(moonSet, horizonState->moonX, MOON_Y);
		} else if (horizonState->moonX < -MOON_WIDTH) {
			horizonState->moonX = SCREEN_WIDTH;
		}
	}

	//obstacles
	if (gameState->spawnObstacles) 
		updateObstacles(scrollSpeed);
	

	REG_BG0HOFS = horizonState->scroll;
}

void updateNight() {
	if (!(horizonState->night) && meterState->invertCounter >= INVERT_DISTANCE) {
		meterState->invertCounter -= INVERT_DISTANCE;
		horizonState->night = true;
		horizonState->inverting = true;
		horizonState->fading = true;
		placeStars();
		horizonState->moonPhase = incrementMoonPhase(moonSet, horizonState->moonPhase);
	} else if ((horizonState->night) && horizonState->invertTimer >= INVERT_FADE_DURATION) {
		horizonState->invertTimer = 0;
		horizonState->night = false;
		horizonState->inverting = true;
		horizonState->fading = true;
	} else if (horizonState->night) {
		horizonState->invertTimer += 1;
	}
}

void placeStars() {
	horizonState->star0X = qran_range(0, SCREEN_WIDTH / 2);
	horizonState->star0Y = qran_range(0, STAR_MAX_Y);
	horizonState->star1X = qran_range(SCREEN_WIDTH / 2, SCREEN_WIDTH);
	horizonState->star1Y = qran_range(0, STAR_MAX_Y);

	setStarPos(starsSet,
		horizonState->star0X,
		horizonState->star0Y,
		horizonState->star1X,
		horizonState->star1Y
	);
}

void updateObstacles(int scrollSpeed) {

	for (int i = 0; i < MAX_OBSTACLES; i++) {
		OBSTACLE * obs = (horizonState->obstacles + i);
		if (obs->visible) {
			updateObstacle(obs, scrollSpeed, i);
			setObstaclePos(obstacleSets + i, obs->type, obs->size, obs->x, obs->y);
		}
	}

	if (horizonState->obstacleCount > 0) {
		OBSTACLE *lastObs = (horizonState->obstacles + (horizonState->lastObstacle));
		if ((horizonState->obstacleCount < MAX_OBSTACLES) && 
			(((lastObs->gap) + (lastObs->x) + (lastObs->width)) < SCREEN_WIDTH)) {
			addObstacle();
		}
	} else {
		addObstacle();
	}
}

void updateObstacle(OBSTACLE * obs, int scrollSpeed, int index) {
	int speedOffset = (obs->speedOffset + obs->extraSpeed) / SPEED_POINT;
	obs->extraSpeed = (obs->speedOffset + obs->extraSpeed) % SPEED_POINT;

	obs->x -= scrollSpeed / SPEED_POINT + speedOffset;
	obs->visible = (obs->x > -(obs->width));
	if (!(obs->visible)) {
		horizonState->obstacleCount -= 1;
		return;
	}

	if (obs->type == PTERODACTYL) {
		obs->frames += 1;
		if (obs->frames == DACTYL_FRAMES) {
			obs->frames = 0;
			obs->size = toggleDactylFlap(obstacleSets + index, obs->size);
		}
	}

}

void addObstacle() {
	OBSTACLE *obs = (OBSTACLE*)(horizonState->obstacles + (horizonState->obstacleCursor));
	switch (qran_range(0,(OBSTACLE_TYPES - (gameState->speed < DACTYL_MIN_SPEED)))) {
		case CACTUS_SMALL:
			createCactusSmall(obs);
			setObstacleSet(obstacleSets + horizonState->obstacleCursor, CACTUS_SMALL, obs->size);
			break;
		case CACTUS_LARGE:
			createCactusLarge(obs);
			setObstacleSet(obstacleSets + horizonState->obstacleCursor, CACTUS_LARGE, obs->size);
			break;
		case PTERODACTYL:
			createPterodactyl(obs);
			setObstacleSet(obstacleSets + horizonState->obstacleCursor, PTERODACTYL, obs->size);
			break;
	}
	setObstaclePos(obstacleSets + (horizonState->obstacleCursor), obs->type, obs->size, obs->x, obs->y);
	horizonState->obstacleCount += 1;
	horizonState->lastObstacle = horizonState->obstacleCursor;
	horizonState->obstacleCursor += 1;
	if (horizonState->obstacleCursor >= MAX_OBSTACLES) 
		horizonState->obstacleCursor = 0;
}

void createCactusSmall(OBSTACLE * obs) {
	obs->type = CACTUS_SMALL;
	obs->x = SCREEN_WIDTH;
	obs->y = CACTUS_SMALL_Y;
	obs->size = (gameState->speed / SPEED_POINT >= CACTUS_SMALL_MULTI_SPEED) ?
		qran_range(1, MAX_OBSTACLE_SIZE + 1) : 1;
	obs->width = CACTUS_SMALL_WIDTH * obs->size;
	obs->height = CACTUS_SMALL_HEIGHT;
	obs->gap = qran_range(((gameState->speed) * (obs->width) + (CACTUS_GAP / 10 * 6)) / SPEED_POINT,
								(CACTUS_GAP + CACTUS_GAP / 2) / SPEED_POINT + 1);
	obs->speedOffset = 0;
	obs->visible = true;

	obs->extraSpeed = 0;

	obs->numBoxes = CACT_COLLISION_BOXES;
	obs->spriteY = obs->y + CACTUS_SMALL_Y_SPRITE_OFFSET;
	cloneBox(obs->colBox, cactSmallBoxes, obs->numBoxes);
	adjustBox(obs->colBox, obs->size, obs->width);
}

void createCactusLarge(OBSTACLE * obs) {
	obs->type = CACTUS_LARGE;
	obs->x = SCREEN_WIDTH;
	obs->y = CACTUS_LARGE_Y;
	obs->size = (gameState->speed / SPEED_POINT >= CACTUS_LARGE_MULTI_SPEED) ?
		qran_range(1, MAX_OBSTACLE_SIZE + 1) : 1;
	obs->width = CACTUS_LARGE_WIDTH * obs->size;
	obs->height = CACTUS_LARGE_HEIGHT;
	obs->gap = qran_range((obs->width * gameState->speed + (CACTUS_GAP / 10 * 6)) / SPEED_POINT,
								(CACTUS_GAP + CACTUS_GAP / 2) / SPEED_POINT + 1);
	obs->speedOffset = 0;
	obs->visible = true;

	obs->extraSpeed = 0;

	obs->numBoxes = CACT_COLLISION_BOXES;
	obs->spriteY = CACTUS_LARGE_Y;
	cloneBox(obs->colBox, cactLargeBoxes, obs->numBoxes);
	adjustBox(obs->colBox, obs->size, obs->width);
}

const int dactylHeights[3] = {100,75,50};

void createPterodactyl(OBSTACLE * obs) {
	obs->type = PTERODACTYL;
	obs->x = SCREEN_WIDTH;
	obs->y = dactylHeights[qran_range(0,3)];
	obs->size = 0;
	obs->width = DACTYL_WIDTH;
	obs->height = DACTYL_HEIGHT;
	obs->gap = qran_range((obs->width * gameState->speed + (DACTYL_GAP / 10 * 6)) / SPEED_POINT,
								(DACTYL_GAP + DACTYL_GAP / 2) / SPEED_POINT + 1);
	obs->speedOffset = (qran_range(0,2)) ? DACTYL_SPEED_OFFSET : -DACTYL_SPEED_OFFSET;
	obs->visible = true;

	obs->frames = 0;
	obs->extraSpeed = 0;

	obs->spriteY = obs->y;

	obs->numBoxes = DACTYL_COLLISION_BOXES;
	cloneBox(obs->colBox, pterodactylBoxes, obs->numBoxes);
}

void adjustBox(COLLISION_BOX * box, int size, int width) {
	if (size > 1) {
		(box + 1)->w = width - box->w - (box + 2)->w;
		(box + 2)->x = width - (box + 2)->w;
	}
}

bool updateDistanceMeter(int distance) {
	int trueDistance = distanceConvert(distance);
	int deltaDistance = (trueDistance - meterState->distance);
	meterState->invertCounter += deltaDistance;
	meterState->achievementCounter += deltaDistance;
	meterState->distance = trueDistance;
	if (meterState->achieving) {
		if (meterState->flashIteration <= FLASH_ITERATIONS) {
			meterState->flashFrame += 1;
			if (meterState->flashFrame == 1) {
				hideNum(scoreSet);
			} else if (meterState->flashFrame == FLASH_FRAMES * 2) {
				meterState->flashFrame = 0;
				meterState->flashIteration += 1;
				showNum(scoreSet);
			} else if (meterState->flashFrame == FLASH_FRAMES) {
				showNum(scoreSet);
			}
		} else {
			meterState->flashIteration = 0;
			meterState->flashFrame = 0;
			meterState->achieving = false;
		}
	} else {
		if (meterState->achievementCounter >= ACHIEVEMENT_DISTANCE) {
			meterState->achievementCounter -= ACHIEVEMENT_DISTANCE;
			meterState->achieving = true;
			setNumValue(scoreSet, trueDistance);
			return true;
		} else {
			setNumValue(scoreSet, trueDistance);
		}
	}
	
	return false;
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

void cloneBox(COLLISION_BOX * clone, const COLLISION_BOX * base, int boxes) {
	for (int i = 0; i < boxes; i++) {
		(clone + i)->x = (base + i)->x;
		(clone + i)->y = (base + i)->y;
		(clone + i)->w = (base + i)->w;
		(clone + i)->h = (base + i)->h;
	}
}

bool boxCheckOffset(const COLLISION_BOX * a, COLLISION_BOX * b,
							int x1, int y1, int x2, int y2) {
	return 	(a->x + x1 < b->x + x2 + b->w &&
				 a->x + x1 + a->w > b->x + x2 &&
				 a->y + y1 < b->y + y2 + b->h &&
				 a->y + y1 + a->h > b->y + y2);
}

bool boxCheck(COLLISION_BOX * a, COLLISION_BOX * b) {
	return 	(a->x < b->x + b->w &&
				 a->x + a->w > b->x &&
				 a->y < b->y + b->h &&
				 a->y + a->h > b->y);
}

void gameOver() {
	mmEffect(SFX_HIT);

	dinoState->status = CRASHED;
	setDinoCrashed(dinoSet);

	gameState->playing = false;

	toggleReplayHide(replaySet);
	showGameover(gameoverSet);

	gameState->gameoverFrames = 0;
}

void init() {
	initSound();
	initMem();
	initGraphics();
	initGame();
}

void initSound() {
	IRQ_INIT();

	irq_add(II_VBLANK, mmVBlank);
	irq_enable(II_VBLANK);
	mmInitDefault((mm_addr)soundbank_bin, 8);
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
  REG_BG0CNT = BG_PRIO(3) | BG_CBB(0) | BG_SBB(31) | BG_4BPP | BG_REG_32x32;
  //curtain layer
  REG_BG1CNT = BG_PRIO(0) | BG_CBB(0) | BG_SBB(29) | BG_4BPP | BG_REG_64x32;

	oam_init(obj_buffer, 128);
	initSets();
	assembleSets();

  REG_DISPCNT= DCNT_OBJ | DCNT_OBJ_2D | DCNT_BG0 | DCNT_BG1;
  REG_BLDCNT = BLD_BUILD(BLD_OBJ, BLD_BG0, BLD_STD);
  REG_BLDALPHA = BLDA_BUILD(0, 16);
  REG_BLDY= BLDY_BUILD(16);

}

void initGame() {
	gameState = malloc(sizeof(GAME_STATE));
	initState(gameState);
	dinoState = malloc(sizeof(DINO_STATE));
	initDino(dinoState);

	horizonState = malloc(sizeof(HORIZON_STATE));
	initHorizon(horizonState);
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		(horizonState->obstacles + i)->visible = false; 
	}

	meterState = malloc(sizeof(METER_STATE));
	initMeter(meterState);
}

void resetGame() {
	toggleReplayHide(replaySet);
	hideGameover(gameoverSet);

	resetState(gameState);
	resetDino(dinoState);
	resetHorizon(horizonState);
	resetObstacles(horizonState->obstacles);
	initMeter(meterState);

	setDinoUpright(dinoSet);
	dinoGraphicsUpdate(dinoSet);

	wipeObstacleSet(obstacleSets);
	wipeObstacleSet(obstacleSets + 1);
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
