#include "dinoSheetHelper.h"
#include "dino.h"
#include "tonc.h"
//Metasprite helper methods

REPLAY_OBJ_SET *createReplaySet(OBJ_ATTR *obj, OBJ_ATTR *obj2)
{
	struct REPLAY_OBJ_SET *set = malloc(sizeof(REPLAY_OBJ_SET));
	set->replay = 
		obj_set_attr(obj, ATTR0_SQUARE, ATTR1_SIZE_32,  replaySI | ATTR2_PALBANK(0));
	set->replayTail = 
		obj_set_attr(obj2, ATTR0_TALL, ATTR1_SIZE_16 | ATTR1_HFLIP, replayTailSI | ATTR2_PALBANK(0));
	return set;
}

void setReplayPos(REPLAY_OBJ_SET *set, int x, int y)
{
	obj_set_pos(set->replay, x, y);
	obj_set_pos(set->replayTail, x + 32, y);
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
		obj_set_attr(index, ATTR0_SQUARE, ATTR1_SIZE_16, num0_SI | ATTR2_PALBANK(0));
	set->num1 =
		obj_set_attr(index + 1, ATTR0_SQUARE, ATTR1_SIZE_16, num0_SI | ATTR2_PALBANK(0));
	set->num2 =
		obj_set_attr(index + 2, ATTR0_SQUARE, ATTR1_SIZE_16, num0_SI | ATTR2_PALBANK(0));
	set->num3 =
		obj_set_attr(index + 3, ATTR0_SQUARE, ATTR1_SIZE_16, num0_SI | ATTR2_PALBANK(0));
	set->num4 =
		obj_set_attr(index + 4, ATTR0_SQUARE, ATTR1_SIZE_16, num0_SI | ATTR2_PALBANK(0));
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

//Give an index, four entries total are used
DINO_OBJ_SET *createDinoSet(OBJ_ATTR *index) {
	struct DINO_OBJ_SET *set = malloc(sizeof(DINO_OBJ_SET));
	set->dinoTorso = 
		obj_set_attr(index + 3, ATTR0_SQUARE, ATTR1_SIZE_32, dinoHeadSI | ATTR2_PALBANK(0));
	set->dinoTail =
		obj_set_attr(index + 1, ATTR0_SQUARE, ATTR1_SIZE_16, dinoTailSI | ATTR2_PALBANK(0));
	set->dinoLegs =
		obj_set_attr(index + 2, ATTR0_WIDE, ATTR1_SIZE_32, dinoFeet1_SI | ATTR2_PALBANK(0));
	set->dinoWink =
		obj_set_attr(index, ATTR0_SQUARE | ATTR0_HIDE, ATTR1_SIZE_8, dinoWinkSI);
	return set;
}

//Set bottom-left dino position
void setDinoPos(DINO_OBJ_SET *set, bool ducking, int x, int y) {
	if (ducking) {

	} else {
		y = 160 - 48 - y;
		obj_set_pos(set->dinoTorso, x + 16, y);
		obj_set_pos(set->dinoTail, x, y + 16);
		obj_set_pos(set->dinoLegs, x, y + 32);
		obj_set_pos(set->dinoWink, x + 24, y);
	}
}

void dinoGraphicsUpdate(DINO_OBJ_SET *set) {

}

//Terrain helpers

void whiteOutBG() {
	se_fill(&se_mem[31][0], blankTile_SI);
}

//x1819
void backgroundInit() {
	for (int i = 0; i < 30; i++) {
		se_plot(&se_mem[31][0], i, 19, 0x2C0 + i);
	}
}