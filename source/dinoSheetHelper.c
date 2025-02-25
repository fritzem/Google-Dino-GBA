#include <stdlib.h>
#include "dinoSheetHelper.h"
#include "dino.h"
#include "tonc.h"

REPLAY_OBJ_SET *replaySet;
BIRD_OBJ_SET *birdSet0;
BIRD_OBJ_SET *birdSet1;
NUM_OBJ_SET *hiScoreSet;
HI_OBJ_SET *hiSet;
NUM_OBJ_SET *scoreSet;
DINO_OBJ_SET *dinoSet;
CLOUD_OBJ_SET *clouds;
MOON_OBJ_SET *moonSet;
STARS_OBJ_SET *starsSet;
OBSTACLE_OBJ_SET *obstacleSets;
GAMEOVER_OBJ_SET *gameoverSet;
TITLE_OBJ_SET *titleSet;
PALETTE_TRACKER *trackers;

//Metasprite helper methods

REPLAY_OBJ_SET *createReplaySet(OBJ_ATTR *obj, OBJ_ATTR *obj2)
{
	struct REPLAY_OBJ_SET *set = malloc(sizeof(REPLAY_OBJ_SET));
	set->replay = 
		obj_set_attr(obj, ATTR0_SQUARE | ATTR0_HIDE, ATTR1_SIZE_32,  replaySI | ATTR2_PALBANK(0));
	set->replayTail = 
		obj_set_attr(obj2, ATTR0_TALL | ATTR0_HIDE, ATTR1_SIZE_16 | ATTR1_HFLIP, replayTailSI | ATTR2_PALBANK(0));
	return set;
}

void setReplayPos(REPLAY_OBJ_SET *set, int x, int y)
{
	obj_set_pos(set->replay, x, y);
	obj_set_pos(set->replayTail, x + 32, y);
}

void toggleReplayHide(REPLAY_OBJ_SET *set)
{
	set->replay->attr0 ^= attr0Visibility;
	set->replayTail->attr0 ^= attr0Visibility;
}

BIRD_OBJ_SET *createBirdSet(OBJ_ATTR *obj, OBJ_ATTR *obj2)
{
	struct BIRD_OBJ_SET *set = malloc(sizeof(BIRD_OBJ_SET));
	set->beak =
		obj_set_attr(obj, ATTR0_SQUARE, ATTR1_SIZE_16, birdBeakSI | ATTR2_PALBANK(0));
	set->birdTorso =
		obj_set_attr(obj2, ATTR0_SQUARE, ATTR1_SIZE_32, birdFlapUpSI | ATTR2_PALBANK(0));
	set->flap = FALSE;
	return set;
}

void setBirdPos(BIRD_OBJ_SET *set, int x, int y)
{
	obj_set_pos(set->beak, x, y);
	obj_set_pos(set->birdTorso, x + 16, y + (set->flap ? 4 : (-4)));
}

void toggleBirdFlap(BIRD_OBJ_SET *set)
{
	set->birdTorso->attr2 ^= birdFlapTrans;
	set->birdTorso->attr0 = ((set->birdTorso->attr0) & 0xFF00) | 
		(u8)(((set->birdTorso->attr0) & 0xFF) + (((-(set->flap) | 1) * 8)));
	set->flap ^= 0x1;
}

//Give an index, five entries total are used
NUM_OBJ_SET *createNumSet(OBJ_ATTR *index)
{
	struct NUM_OBJ_SET *set = malloc(sizeof(NUM_OBJ_SET));
	set->num0 =
		obj_set_attr(index, ATTR0_SQUARE, ATTR1_SIZE_16, num0_SI | ATTR2_PALBANK(0) | ATTR2_PRIO(1));
	set->num1 =
		obj_set_attr(index + 1, ATTR0_SQUARE, ATTR1_SIZE_16, num0_SI | ATTR2_PALBANK(0) | ATTR2_PRIO(1));
	set->num2 =
		obj_set_attr(index + 2, ATTR0_SQUARE, ATTR1_SIZE_16, num0_SI | ATTR2_PALBANK(0) | ATTR2_PRIO(1));
	set->num3 =
		obj_set_attr(index + 3, ATTR0_SQUARE, ATTR1_SIZE_16, num0_SI | ATTR2_PALBANK(0) | ATTR2_PRIO(1));
	set->num4 =
		obj_set_attr(index + 4, ATTR0_SQUARE, ATTR1_SIZE_16, num0_SI | ATTR2_PALBANK(0) | ATTR2_PRIO(1));
	return set;
}

void setNumPos(NUM_OBJ_SET *set, int x, int y)
{
	obj_set_pos(set->num0, x, y);
	obj_set_pos(set->num1, x + 11, y);
	obj_set_pos(set->num2, x + 22, y);
	obj_set_pos(set->num3, x + 33, y);
	obj_set_pos(set->num4, x + 44, y);
}

void setNumValue(NUM_OBJ_SET *set, int num)
{
	int val = num % 10;
	BFN_SET(set->num4->attr2, numToSI(val), ATTR2_ID);
	val = (num % 100 - val) / 10;
	BFN_SET(set->num3->attr2, numToSI(val), ATTR2_ID);
	val = (num % 1000 - val) / 100;
	BFN_SET(set->num2->attr2, numToSI(val), ATTR2_ID);
	val = (num % 10000 - val) / 1000;
	BFN_SET(set->num1->attr2, numToSI(val), ATTR2_ID);
	val = (num % 100000 - val) / 10000;
	BFN_SET(set->num0->attr2, numToSI(val), ATTR2_ID);
}

void hideNum(NUM_OBJ_SET *set) {
	obj_hide(set->num0);
	obj_hide(set->num1);
	obj_hide(set->num2);
	obj_hide(set->num3);
	obj_hide(set->num4);
}

void showNum(NUM_OBJ_SET *set) {
	obj_unhide_multi(set->num0, ATTR0_REG, 5);
}

HI_OBJ_SET *createHiSet(OBJ_ATTR *obj0, OBJ_ATTR *obj1)
{
	struct HI_OBJ_SET *set = malloc(sizeof(HI_OBJ_SET));
	set->hi0 =
		obj_set_attr(obj0, ATTR0_SQUARE, ATTR1_SIZE_16, hiSI | ATTR2_PALBANK(0) | ATTR2_PRIO(1));
	set->hi1 =
		obj_set_attr(obj1, ATTR0_TALL, ATTR1_SIZE_8, (hiSI + 2) | ATTR2_PALBANK(0) | ATTR2_PRIO(1));
	return set;
}

void setHiPos(HI_OBJ_SET *set, int x, int y) {
	obj_set_pos(set->hi0, x, y);
	obj_set_pos(set->hi1, x + 16, y);
}

//Give an index, four entries total are used
DINO_OBJ_SET *createDinoSet(OBJ_ATTR *index) {
	struct DINO_OBJ_SET *set = malloc(sizeof(DINO_OBJ_SET));
	set->dinoTorso = 
		obj_set_attr(index + 3, ATTR0_SQUARE, ATTR1_SIZE_32, dinoHeadSI | ATTR2_PALBANK(0));
	set->dinoTail =
		obj_set_attr(index + 1, ATTR0_SQUARE, ATTR1_SIZE_16, dinoTailSI | ATTR2_PALBANK(0));
	set->dinoLegs =
		obj_set_attr(index + 2, ATTR0_WIDE, ATTR1_SIZE_32, dinoFeet0_SI | ATTR2_PALBANK(0));
	set->dinoWink =
		obj_set_attr(index, ATTR0_SQUARE | ATTR0_HIDE, ATTR1_SIZE_8, dinoWinkSI);
	return set;
}

//Set bottom-left dino position (only upright)
void setDinoPos(DINO_OBJ_SET *set, int x, int y) {
	y = 160 - 48 - y;
	obj_set_pos(set->dinoTorso, x + 16, y);
	obj_set_pos(set->dinoTail, x, y + 16);
	obj_set_pos(set->dinoLegs, x, y + 32);
	obj_set_pos(set->dinoWink, x + 24, y);
}

void dinoGraphicsUpdate(DINO_OBJ_SET *set) {
	int y = SCREEN_HEIGHT - 48 - dinoState->yPos;
	int crouchY = SCREEN_HEIGHT - 32 - dinoState->yPos;
	switch (dinoState->status) {
		case CRASHED:
			obj_set_pos(set->dinoTorso, dinoState->xPos - 22, y);
			break;
		case DUCKING:
			obj_set_pos(set->dinoTorso, dinoState->xPos, crouchY);
			obj_set_pos(set->dinoLegs, dinoState->xPos + 8, crouchY + 16);
			break;
		case JUMPING:
		case RUNNING:
		case WAITING:
			obj_set_pos(set->dinoTorso, dinoState->xPos + 16, y);
			obj_set_pos(set->dinoTail, dinoState->xPos, y + 16);
			obj_set_pos(set->dinoLegs, dinoState->xPos, y + 32);
			obj_set_pos(set->dinoWink, dinoState->xPos + 24, y);
			break;
	}
}

void setDinoUpright(DINO_OBJ_SET *set) {
	obj_set_attr(set->dinoTorso, ATTR0_SQUARE, ATTR1_SIZE_32, dinoHeadSI | ATTR2_PALBANK(0));
	obj_set_attr(set->dinoTail, ATTR0_SQUARE, ATTR1_SIZE_16, dinoTailSI | ATTR2_PALBANK(0));
	obj_set_attr(set->dinoLegs, ATTR0_WIDE, ATTR1_SIZE_32, dinoFeet0_SI | ATTR2_PALBANK(0));
	obj_set_attr(set->dinoWink, ATTR0_SQUARE | ATTR0_HIDE, ATTR1_SIZE_8, dinoWinkSI | ATTR2_PALBANK(0));
}

void setDinoAnim(DINO_OBJ_SET *set, int si) {
	obj_set_attr(set->dinoLegs, ATTR0_WIDE, ATTR1_SIZE_32, si | ATTR2_PALBANK(0));
}

void setDinoDucking(DINO_OBJ_SET *set) {
	obj_set_attr(set->dinoTorso, ATTR0_WIDE, ATTR1_SIZE_64, dinoCrouchSI | ATTR2_PALBANK(0));
	obj_set_attr(set->dinoLegs, ATTR0_WIDE, ATTR1_SIZE_32, dinoCrouchFeet1_SI | ATTR2_PALBANK(0));
	obj_hide(set->dinoTail);
}

void setDinoCrashed(DINO_OBJ_SET *set) {
	obj_set_attr(set->dinoTorso, ATTR0_SQUARE, ATTR1_SIZE_64, deadDinoSI | ATTR2_PALBANK(0));
	obj_hide(set->dinoLegs);
	obj_hide(set->dinoTail);
}

void dinoBlink(DINO_OBJ_SET *set) {
	obj_unhide(set->dinoWink, ATTR0_REG);
}

void dinoUnBlink(DINO_OBJ_SET *set) {
	obj_hide(set->dinoWink);
}

//Give an index, twelve entries total are used
CLOUD_OBJ_SET *createCloudSets(OBJ_ATTR *index) {
	CLOUD_OBJ_SET* clouds = malloc(2 * MAX_CLOUDS * sizeof(CLOUD_OBJ_SET));
	for (int i = 0; i < MAX_CLOUDS * 2; i += 2) {
		(clouds + (i / 2))->cloudL =
			obj_set_attr(index + i, ATTR0_WIDE, ATTR1_SIZE_32 | ATTR1_X(SCREEN_WIDTH), cloudLeftSI | ATTR2_PALBANK(0));
		(clouds + (i / 2))->cloudR =
			obj_set_attr(index + i + 1, ATTR0_SQUARE, ATTR1_SIZE_16 | ATTR1_X(SCREEN_WIDTH), cloudRightSI | ATTR2_PALBANK(0));
	}
	return clouds;
}

void setCloudPos(CLOUD_OBJ_SET *set, int x, int y) {
	obj_set_pos(set->cloudL, x, y);
	obj_set_pos(set->cloudR, x + 32, y);
}

STARS_OBJ_SET *createStarsSet(OBJ_ATTR *index) {
	struct STARS_OBJ_SET *set = malloc(sizeof(STARS_OBJ_SET));
	set->star0 =
		obj_set_attr(index, ATTR0_SQUARE | ATTR0_BLEND, ATTR1_SIZE_16, star0_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(0));
	set->star1 =
		obj_set_attr(index + 1, ATTR0_SQUARE | ATTR0_BLEND, ATTR1_SIZE_16, star1_SI | ATTR2_PRIO(2) |ATTR2_PALBANK(0));
	return set;
}

void setStarTypes(STARS_OBJ_SET *set, int star0, int star1) {
	BFN_SET(set->star0->attr2, starSI(star0), ATTR2_ID);
	BFN_SET(set->star1->attr2, starSI(star1), ATTR2_ID);
}

void setStarPos(STARS_OBJ_SET *set, int x0, int y0, int x1, int y1) {
	obj_set_pos(set->star0, x0, y0);
	obj_set_pos(set->star1, x1, y1);
}

MOON_OBJ_SET *createMoonSet(OBJ_ATTR *index) {
	struct MOON_OBJ_SET *set = malloc(sizeof(MOON_OBJ_SET));
	set->moonChunk0 =
		obj_set_attr(index, ATTR0_SQUARE | ATTR0_BLEND, ATTR1_SIZE_32 | ATTR1_HFLIP, moon1_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(0));
	set->moonChunk1 =
		obj_set_attr(index + 1, ATTR0_SQUARE | ATTR0_BLEND, ATTR1_SIZE_32 | ATTR1_HFLIP | ATTR1_VFLIP, moon1_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(0));
	set->moonChunk2 = 
		obj_set_attr(index + 2, ATTR0_WIDE | ATTR0_BLEND | ATTR0_HIDE, ATTR1_SIZE_32, fullMoonTopSI | ATTR2_PRIO(2) | ATTR2_PALBANK(0));
	set->moonChunk3 = 
		obj_set_attr(index + 3, ATTR0_SQUARE | ATTR0_BLEND | ATTR0_HIDE, ATTR1_SIZE_32, fullMoonSI | ATTR2_PRIO(2) | ATTR2_PALBANK(0));
	return set;
}

int incrementMoonPhase(MOON_OBJ_SET *set, int currentPhase) {
	switch (currentPhase) {
		case 0:
			BFN_SET(set->moonChunk0->attr2, moon1_SI, ATTR2_ID);
			BFN_SET(set->moonChunk1->attr2, moon1_SI, ATTR2_ID);
			return 1;
		case 1:
			BFN_SET(set->moonChunk0->attr2, moon2_SI, ATTR2_ID);
			BFN_SET(set->moonChunk1->attr2, moon2_SI, ATTR2_ID);
			return 2;
		case 2:
			BFN_SET(set->moonChunk0->attr1, 0, ATTR1_FLIP);
			BFN_SET(set->moonChunk1->attr1, 2, ATTR1_FLIP);
			BFN_SET(set->moonChunk0->attr0, ATTR0_TALL, ATTR0_SHAPE);
			BFN_SET(set->moonChunk1->attr0, ATTR0_TALL, ATTR0_SHAPE);
			set->moonChunk2->attr0 ^= attr0Visibility;
			set->moonChunk3->attr0 ^= attr0Visibility;
			return 3;
		case 3:
			BFN_SET(set->moonChunk0->attr0, ATTR0_SQUARE, ATTR0_SHAPE);
			BFN_SET(set->moonChunk1->attr0, ATTR0_SQUARE, ATTR0_SHAPE);
			set->moonChunk2->attr0 ^= attr0Visibility;
			set->moonChunk3->attr0 ^= attr0Visibility;
			return 4;
		case 4:
			BFN_SET(set->moonChunk0->attr2, moon1_SI, ATTR2_ID);
			BFN_SET(set->moonChunk1->attr2, moon1_SI, ATTR2_ID);
			return 5;
		case 5:
			BFN_SET(set->moonChunk0->attr2, moon0_SI, ATTR2_ID);
			BFN_SET(set->moonChunk1->attr2, moon0_SI, ATTR2_ID);
			return 6;
		case 6:
			BFN_SET(set->moonChunk0->attr1, 1, ATTR1_FLIP);
			BFN_SET(set->moonChunk1->attr1, 3, ATTR1_FLIP);
			return 0;
		default:
			return 1;
	}
	return 0;
}

void setMoonPos(MOON_OBJ_SET *set, int x, int y) {
	obj_set_pos(set->moonChunk0, x, y);
	obj_set_pos(set->moonChunk1, x, y + 8);
	obj_set_pos(set->moonChunk2, x + 8, y);
	obj_set_pos(set->moonChunk3, x + 8, y + 8);
}

OBSTACLE_OBJ_SET *createObstacleSets(OBJ_ATTR *index) {
	OBSTACLE_OBJ_SET *set = malloc(MAX_OBSTACLES * sizeof(OBSTACLE_OBJ_SET));
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		(set + i)->obstacleChunk0 =
			obj_set_attr(index + i * 4, ATTR0_HIDE, 0, 0);
		(set + i)->obstacleChunk1 =
			obj_set_attr(index + i * 4 + 1, ATTR0_HIDE, 0, 0);
		(set + i)->obstacleChunk2 =
			obj_set_attr(index + i * 4 + 2, ATTR0_HIDE, 0, 0);
		(set + i)->obstacleChunk3 =
			obj_set_attr(index + i * 4 + 3, ATTR0_HIDE, 0, 0);
	}
	return set;
}

void wipeObstacleSet(OBSTACLE_OBJ_SET *set) {
	obj_set_attr(set->obstacleChunk0, ATTR0_HIDE, 0, 0);
	obj_set_attr(set->obstacleChunk1, ATTR0_HIDE, 0, 0);
	obj_set_attr(set->obstacleChunk2, ATTR0_HIDE, 0, 0);
	obj_set_attr(set->obstacleChunk3, ATTR0_HIDE, 0, 0);
}

void setObstacleSet(OBSTACLE_OBJ_SET *set, int type, int size) {
	wipeObstacleSet(set);
	switch (type * TYPEM + size) {
		case CACTUS_SMALL_0:
			obj_set_attr(set->obstacleChunk0, ATTR0_SQUARE, ATTR1_SIZE_32, smallCactusSingleSI | ATTR2_PALBANK(0));
			obj_set_attr(set->obstacleChunk1, ATTR0_SQUARE, ATTR1_SIZE_8, smallCactusStumpSI | ATTR2_PALBANK(0));
			break;
		case CACTUS_SMALL_1:
			obj_set_attr(set->obstacleChunk0, ATTR0_WIDE, ATTR1_SIZE_64, smallCactusDoubleSI | ATTR2_PALBANK(0));
			obj_set_attr(set->obstacleChunk1, ATTR0_SQUARE, ATTR1_SIZE_8, smallCactusStumpSI | ATTR2_PALBANK(0));
			obj_set_attr(set->obstacleChunk2, ATTR0_SQUARE, ATTR1_SIZE_8, smallCactusStumpSI | ATTR2_PALBANK(0));
			break;
		case CACTUS_SMALL_2:
			obj_set_attr(set->obstacleChunk0, ATTR0_WIDE, ATTR1_SIZE_64, smallCactusTripleSI | ATTR2_PALBANK(0));
			obj_set_attr(set->obstacleChunk1, ATTR0_SQUARE, ATTR1_SIZE_8, smallCactusStumpSI | ATTR2_PALBANK(0));
			obj_set_attr(set->obstacleChunk2, ATTR0_SQUARE, ATTR1_SIZE_8, smallCactusStumpSI | ATTR2_PALBANK(0));
			obj_set_attr(set->obstacleChunk3, ATTR0_SQUARE, ATTR1_SIZE_8, smallCactusStumpSI | ATTR2_PALBANK(0));
			break;
		case CACTUS_LARGE_0:
			obj_set_attr(set->obstacleChunk0, ATTR0_TALL, ATTR1_SIZE_64, bigCactusSingleSI | ATTR2_PALBANK(0));
			break;
		case CACTUS_LARGE_1:
			obj_set_attr(set->obstacleChunk0, ATTR0_SQUARE, ATTR1_SIZE_64, bigCactusDoubleSI | ATTR2_PALBANK(0));
			break;
		case CACTUS_LARGE_2:
			obj_set_attr(set->obstacleChunk0, ATTR0_SQUARE, ATTR1_SIZE_64, bigCactusTripleSI | ATTR2_PALBANK(0));
			obj_set_attr(set->obstacleChunk1, ATTR0_TALL, ATTR1_SIZE_64, bigCactusTripleRunoffSI | ATTR2_PALBANK(0));
			break;
		case PTERODACTYL_0:
			obj_set_attr(set->obstacleChunk0, ATTR0_SQUARE, ATTR1_SIZE_16, birdBeakSI | ATTR2_PALBANK(0));
			obj_set_attr(set->obstacleChunk1, ATTR0_SQUARE, ATTR1_SIZE_32, birdFlapUpSI | ATTR2_PALBANK(0));
			break;
	}
}

//Use size parameter for flap if dactyl
void setObstaclePos(OBSTACLE_OBJ_SET *set, int type, int size, int x, int y) {
	switch (type * TYPEM + size) {
		case CACTUS_SMALL_0:
			obj_set_pos(set->obstacleChunk0, x - 15, CACTUS_SMALL_Y);
			obj_set_pos(set->obstacleChunk1, x + 5, CACTUS_SMALL_Y + 32);
			break;
		case CACTUS_SMALL_1:
			obj_set_pos(set->obstacleChunk0, x - 15, CACTUS_SMALL_Y);
			obj_set_pos(set->obstacleChunk1, x + 5, CACTUS_SMALL_Y + 32);
			obj_set_pos(set->obstacleChunk2, x + 22, CACTUS_SMALL_Y + 32);
			break;
		case CACTUS_SMALL_2:
			obj_set_pos(set->obstacleChunk0, x - 8, CACTUS_SMALL_Y);
			obj_set_pos(set->obstacleChunk1, x + 5, CACTUS_SMALL_Y + 32);
			obj_set_pos(set->obstacleChunk2, x + 22, CACTUS_SMALL_Y + 32);
			obj_set_pos(set->obstacleChunk3, x + 39, CACTUS_SMALL_Y + 32);
			break;
		case CACTUS_LARGE_0:
		case CACTUS_LARGE_1:
			obj_set_pos(set->obstacleChunk0, x, CACTUS_LARGE_Y);
			break;
		case CACTUS_LARGE_2:
			obj_set_pos(set->obstacleChunk0, x, CACTUS_LARGE_Y);
			obj_set_pos(set->obstacleChunk1, x + 64, CACTUS_LARGE_Y);
			break;
		case PTERODACTYL_0:
		case PTERODACTYL_1:
			obj_set_pos(set->obstacleChunk0, x, y);
			obj_set_pos(set->obstacleChunk1, x + 16, y + (size ? 4 : (-4)));
			break;
	}
}

bool toggleDactylFlap(OBSTACLE_OBJ_SET *set, bool flap)
{
	set->obstacleChunk1->attr2 ^= birdFlapTrans;
	set->obstacleChunk1->attr0 = ((set->obstacleChunk1->attr0) & 0xFF00) | 
		(u8)(((set->obstacleChunk1->attr0) & 0xFF) + (((-(flap) | 1) * 8)));
	return flap ^ 0x1;
}

GAMEOVER_OBJ_SET *createGameoverSet(OBJ_ATTR *index) {
	GAMEOVER_OBJ_SET * set = malloc(sizeof(GAMEOVER_OBJ_SET));

	set->g = obj_set_attr(index, ATTR0_Y(charY) | ATTR0_SQUARE | ATTR0_HIDE,
								ATTR1_X(charXg) | ATTR1_SIZE_16, 
								charG_SI | ATTR2_PALBANK(0));
	set->a = obj_set_attr(index + 1, ATTR0_Y(charY) | ATTR0_SQUARE | ATTR0_HIDE,
								ATTR1_X(charXa) | ATTR1_SIZE_16, 
								charA_SI | ATTR2_PALBANK(0));
	set->m = obj_set_attr(index + 2, ATTR0_Y(charY) | ATTR0_SQUARE | ATTR0_HIDE,
								ATTR1_X(charXm) | ATTR1_SIZE_16, 
								charM_SI | ATTR2_PALBANK(0));
	set->e = obj_set_attr(index + 3, ATTR0_Y(charY) | ATTR0_SQUARE | ATTR0_HIDE,
								ATTR1_X(charXe) | ATTR1_SIZE_16, 
								charE_SI | ATTR2_PALBANK(0));
	set->o = obj_set_attr(index + 4, ATTR0_Y(charY) | ATTR0_SQUARE | ATTR0_HIDE,
								ATTR1_X(charXo) | ATTR1_SIZE_16, 
								charO_SI | ATTR2_PALBANK(0));
	set->v = obj_set_attr(index + 5, ATTR0_Y(charY) | ATTR0_SQUARE | ATTR0_HIDE,
								ATTR1_X(charXv) | ATTR1_SIZE_16, 
								charV_SI | ATTR2_PALBANK(0));
	set->e2 = obj_set_attr(index + 6, ATTR0_Y(charY) | ATTR0_SQUARE | ATTR0_HIDE,
								ATTR1_X(charXe2) | ATTR1_SIZE_16, 
								charE_SI | ATTR2_PALBANK(0));
	set->r = obj_set_attr(index + 7, ATTR0_Y(charY) | ATTR0_SQUARE | ATTR0_HIDE,
								ATTR1_X(charXr) | ATTR1_SIZE_16, 
								charR_SI | ATTR2_PALBANK(0));
	return set;
}

void showGameover(GAMEOVER_OBJ_SET * set) {
	obj_unhide_multi(set->g, ATTR0_REG, 8);
}

void hideGameover(GAMEOVER_OBJ_SET * set) {
	obj_hide_multi(set->g, 8);
}

TITLE_OBJ_SET *createTitleSet(OBJ_ATTR *index) {
	TITLE_OBJ_SET * set = malloc(sizeof(TITLE_OBJ_SET));

	set->left = obj_set_attr(index, ATTR0_Y(40) | ATTR0_WIDE,
									ATTR1_SIZE_64,  titleLeftSI | ATTR2_PALBANK(1));
	set->right = obj_set_attr(index + 1, ATTR0_Y(40) | ATTR0_WIDE,
									64 | ATTR1_SIZE_64, titleRightSI | ATTR2_PALBANK(1));
	return set;
}

void hideTitle(TITLE_OBJ_SET * set) {
	obj_hide_multi(set->left, 2);
}

void initSets() {
	scoreSet = createNumSet(&obj_buffer[0]);
	hiScoreSet = createNumSet(&obj_buffer[5]);
	hiSet = createHiSet(&obj_buffer[10], &obj_buffer[11]);
	replaySet = createReplaySet(&obj_buffer[12],&obj_buffer[13]);
	gameoverSet = createGameoverSet(&obj_buffer[14]);
	
	dinoSet = createDinoSet(&obj_buffer[22]);
	obstacleSets = createObstacleSets(&obj_buffer[26]);

	clouds = createCloudSets(&obj_buffer[34]);
	moonSet = createMoonSet(&obj_buffer[46]);
	starsSet = createStarsSet(&obj_buffer[50]);
	
	titleSet = createTitleSet(&obj_buffer[52]);

	
	

	trackers = createTrackers();
}

void assembleSets() {
	setReplayPos(replaySet, 102, 70);
  	setNumPos(hiScoreSet, 107, 10);
  	setHiPos(hiSet, 74, 10);
  	setNumPos(scoreSet, 174, 10);
  	setMoonPos(moonSet, SCREEN_WIDTH - 50, MOON_Y);
}

//Terrain helpers

void whiteOutBG() {
	se_fill(&se_mem[29][0], blankTile_SI);
    se_fill(&se_mem[30][0], transparent_SI);
    se_fill(&se_mem[31][0], blankTile_SI);
}

//x1819
void backgroundInit() {
	REG_BG1HOFS = STARTING_CURTAIN_SCROLL;

	for (int i = 0; i < 31; i++) {
		se_plot(&se_mem[31][0], i, GROUND_Y, firstTerrain_SI + i);
	}
}

void updateHorizonTile(int bg_index, int terrainIndex, bool bumpy) {
	int top_SI = blankTile_SI;
	if (bumpy && (terrainIndex == 16 || terrainIndex == 17 
			|| terrainIndex == 20 || terrainIndex == 21
			|| (terrainIndex >= 56 && terrainIndex <= 61))) {
		top_SI = firstTerrain_SI + terrainIndex + BUMPY_TOP_OFFSET;
		terrainIndex += BUMPY_OFFSET;
	}
    se_plot(&se_mem[31][0], bg_index, GROUND_Y - 1, top_SI);
    se_plot(&se_mem[31][0], bg_index, GROUND_Y, firstTerrain_SI + terrainIndex);
}

void invertPalettes() {
	for (u16 palIndex = 1; palIndex < 8; palIndex++) {
		int red = 31 - ((*(pal_obj_mem + palIndex) & RED_MASK) >> RED_SHIFT);
		int green = 31 - ((*(pal_obj_mem + palIndex) & GREEN_MASK) >> GREEN_SHIFT);
		int blue = 31 - ((*(pal_obj_mem + palIndex) & BLUE_MASK) >> BLUE_SHIFT);
		*(pal_obj_mem + palIndex) = RGB15(red, green, blue);
		*(pal_bg_mem + palIndex) = RGB15(red, green, blue);
	}
}

PALETTE_TRACKER *createTrackers() {
	PALETTE_TRACKER* trackers = malloc(8 * sizeof(PALETTE_TRACKER));
	for (u16 palIndex = 1; palIndex < 8; palIndex++) {
		(trackers + palIndex)->maxRed = ((*(pal_obj_mem + palIndex) & RED_MASK) >> RED_SHIFT);
		(trackers + palIndex)->minRed = 31 - ((*(pal_obj_mem + palIndex) & RED_MASK) >> RED_SHIFT);
		(trackers + palIndex)->curRed = ((*(pal_obj_mem + palIndex) & RED_MASK) >> RED_SHIFT) * PALETTE_POINT;
		(trackers + palIndex)->incRed = ((trackers + palIndex)->minRed - (trackers + palIndex)->maxRed) * PALETTE_POINT / INVERT_FRAMES;

		(trackers + palIndex)->maxGreen = ((*(pal_obj_mem + palIndex) & GREEN_MASK) >> GREEN_SHIFT);
		(trackers + palIndex)->minGreen = 31 - ((*(pal_obj_mem + palIndex) & GREEN_MASK) >> GREEN_SHIFT);
		(trackers + palIndex)->curGreen = ((*(pal_obj_mem + palIndex) & GREEN_MASK) >> GREEN_SHIFT) * PALETTE_POINT;
		(trackers + palIndex)->incGreen = ((trackers + palIndex)->minGreen - (trackers + palIndex)->maxGreen) * PALETTE_POINT / INVERT_FRAMES;

		(trackers + palIndex)->maxBlue = ((*(pal_obj_mem + palIndex) & BLUE_MASK) >> BLUE_SHIFT);
		(trackers + palIndex)->minBlue = 31 - ((*(pal_obj_mem + palIndex) & BLUE_MASK) >> BLUE_SHIFT);
		(trackers + palIndex)->curBlue = ((*(pal_obj_mem + palIndex) & BLUE_MASK) >> BLUE_SHIFT) * PALETTE_POINT;
		(trackers + palIndex)->incBlue = ((trackers + palIndex)->minBlue - (trackers + palIndex)->maxBlue) * PALETTE_POINT / INVERT_FRAMES;
	}
	return trackers;
}

void inversionUpdate(bool night, bool invertOver) {
	if (invertOver) {
		int red = 0;
		int green = 0;
		int blue = 0;
		for (u16 palIndex = 1; palIndex < 8; palIndex++) {
			if (night) {
				red = (trackers + palIndex)->minRed;
				green = (trackers + palIndex)->minGreen;
				blue = (trackers + palIndex)->minBlue;
			} else {
				red = (trackers + palIndex)->maxRed;
				green = (trackers + palIndex)->maxGreen;
				blue = (trackers + palIndex)->maxBlue;
			}
			*(pal_obj_mem + palIndex) = RGB15(red, green, blue);
			*(pal_bg_mem + palIndex) = RGB15(red, green, blue);
		}
	} else if (night) {
		for (u16 palIndex = 1; palIndex < 8; palIndex++) {
			(trackers + palIndex)->curRed += (trackers + palIndex)->incRed;
			(trackers + palIndex)->curGreen += (trackers + palIndex)->incGreen;
			(trackers + palIndex)->curBlue += (trackers + palIndex)->incBlue;
			int red = (trackers + palIndex)->curRed / PALETTE_POINT;
			int green = (trackers + palIndex)->curGreen / PALETTE_POINT;
			int blue = (trackers + palIndex)->curBlue / PALETTE_POINT;
			*(pal_obj_mem + palIndex) = RGB15(red, green, blue);
			*(pal_bg_mem + palIndex) = RGB15(red, green, blue);
		}
	} else {
		for (u16 palIndex = 1; palIndex < 8; palIndex++) {
			(trackers + palIndex)->curRed -= (trackers + palIndex)->incRed;
			(trackers + palIndex)->curGreen -= (trackers + palIndex)->incGreen;
			(trackers + palIndex)->curBlue -= (trackers + palIndex)->incBlue;
			int red = (trackers + palIndex)->curRed / PALETTE_POINT;
			int green = (trackers + palIndex)->curGreen / PALETTE_POINT;
			int blue = (trackers + palIndex)->curBlue / PALETTE_POINT;
			*(pal_obj_mem + palIndex) = RGB15(red, green, blue);
			*(pal_bg_mem + palIndex) = RGB15(red, green, blue);
		}
	}
	
}