#ifndef DINO_SHEET_HELPER_H
#define DINO_SHEET_HELPER_H

#include <stdlib.h>
#include "tonc.h"

//Sprite Indices
#define replaySI 0x0
#define replayTailSI 0x0

#define cloudSI 0x4

#define smallCactusSingleSI 0xA
#define smallCactusDoubleSI 0xA
#define smallCactusTripleSI 0x12
#define smallCactusStumpSI 0xC8

#define bigCactusSingleSI 0x180
#define bigCactusDoubleSI 0x184
#define bigCactusTripleSI 0x1CC
#define bigCactusTripleRunoffSI 0x1D4

#define dinoHeadSI 0x1A
#define dinoWinkSI 0x1E
#define dinoDeadeyeSI 0x1F
#define dinoTailSI 0x5E
#define dinoFeet1_SI 0x10C
#define dinoFeet2_SI 0x112
#define dinoFeet3_SI 0x116
#define dinoCrouchSI 0x14C
#define dinoCrouchFeet1_SI 0x11A
#define dinoCrouchFeet2_SI 0x15A
#define deadDinoSI 0x196

#define birdBeakSI 0x44
#define birdFlapDownSI 0x80
#define birdFlapUpSI 0x84
#define birdFlapTrans (birdFlapDownSI ^ birdFlapUpSI)

#define star1_SI 0x46
#define star2_SI 0x48
#define star3_SI 0x88

#define fullMoonTopSI 0xE8
#define moon1_SI 0x100
#define moon2_SI 0x104
#define moon3_SI 0x1068
#define fullMoonSI 0x10C

#define num0_SI 0x8A
#define num1_SI 0x8C
#define num2_SI 0x8E
#define num3_SI 0x90
#define num4_SI 0x92
#define num5_SI 0x94
#define num6_SI 0x96
#define num7_SI 0x98
#define num8_SI 0x9A
#define num9_SI 0x9C
INLINE int numToSI(int x) {return num0_SI + (x * 0x2);}

#define charG_SI 0xCC
#define charA_SI 0xCE
#define charM_SI 0xD0
#define charE_SI 0xD2
#define charO_SI 0xD4
#define charV_SI 0xD6
#define charR_SI 0xD8

#define hiSI 0xDA

//Metasprite helper methods

typedef struct REPLAY_OBJ_SET {
	OBJ_ATTR* replay;
	OBJ_ATTR* replayTail;
} REPLAY_OBJ_SET, REPLAY_OBJ_SET;

INLINE REPLAY_OBJ_SET *createReplaySet(OBJ_ATTR *obj, OBJ_ATTR *obj2)
{
	struct REPLAY_OBJ_SET *set = malloc(sizeof(REPLAY_OBJ_SET));
	set->replay = 
		obj_set_attr(obj, ATTR0_SQUARE, ATTR1_SIZE_32,  replaySI | ATTR2_PALBANK(0));
	set->replayTail = 
		obj_set_attr(obj2, ATTR0_TALL, ATTR1_SIZE_16 | ATTR1_HFLIP, replayTailSI | ATTR2_PALBANK(0));
	return set;
}

INLINE void setReplayPos(REPLAY_OBJ_SET *set, int x, int y)
{
	obj_set_pos(set->replay, x, y);
	obj_set_pos(set->replayTail, x + 32, y);
}

typedef struct BIRD_OBJ_SET {
	OBJ_ATTR* beak;
	OBJ_ATTR* birdTorso;
	BOOL flap;
} BIRD_OBJ_SET, BIRD_OBJ_SET;

INLINE BIRD_OBJ_SET *createBirdSet(OBJ_ATTR *obj, OBJ_ATTR *obj2)
{
	struct BIRD_OBJ_SET *set = malloc(sizeof(BIRD_OBJ_SET));
	set->beak =
		obj_set_attr(obj, ATTR0_SQUARE, ATTR1_SIZE_16, birdBeakSI | ATTR2_PALBANK(0));
	set->birdTorso =
		obj_set_attr(obj2, ATTR0_SQUARE, ATTR1_SIZE_32, birdFlapUpSI | ATTR2_PALBANK(0));
	set->flap = FALSE;
	return set;
}

INLINE void setBirdPos(BIRD_OBJ_SET *set, int x, int y)
{
	obj_set_pos(set->beak, x, y);
	obj_set_pos(set->birdTorso, x + 16, y + (set->flap ? 4 : (-4)));
}

INLINE void toggleBirdFlap(BIRD_OBJ_SET *set)
{
	set->birdTorso->attr2 ^= birdFlapTrans;
	set->birdTorso->attr0 = ((set->birdTorso->attr0) & 0xFF00) | 
		(u8)(((set->birdTorso->attr0) & 0xFF) + (((-(set->flap) | 1) * 8)));
	set->flap ^= 0x1;
}

//Used for both score and hiscore
typedef struct NUM_OBJ_SET {
	OBJ_ATTR* num0;
	OBJ_ATTR* num1;
	OBJ_ATTR* num2;
	OBJ_ATTR* num3;
	OBJ_ATTR* num4;
} NUM_OBJ_SET, NUM_OBJ_SET;

//Give an index, five entries total are used
INLINE NUM_OBJ_SET *createNumSet(OBJ_ATTR *index)
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

INLINE void setNumPos(NUM_OBJ_SET *set, int x, int y)
{
	obj_set_pos(set->num0, x, y);
	obj_set_pos(set->num1, x + 11, y);
	obj_set_pos(set->num2, x + 22, y);
	obj_set_pos(set->num3, x + 33, y);
	obj_set_pos(set->num4, x + 44, y);
}

INLINE void setNumValue(NUM_OBJ_SET *set, int num)
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




#endif