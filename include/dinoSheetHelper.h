#ifndef DINO_SHEET_HELPER_H
#define DINO_SHEET_HELPER_H

#include <stdlib.h>
#include "tonc.h"

#define attr0Visibility 0x200

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
#define dinoFeet0_SI 0x110
#define dinoFeet1_SI 0x114
#define dinoFeet2_SI 0x118
#define dinoCrouchSI 0x150
#define dinoCrouchFeet0_SI 0x11C
#define dinoCrouchFeet1_SI 0x15C
#define deadDinoSI 0x198

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

//Terrain 

#define blankTile_SI 0x359

//Metasprite structs

typedef struct REPLAY_OBJ_SET {
	OBJ_ATTR* replay;
	OBJ_ATTR* replayTail;
} REPLAY_OBJ_SET, REPLAY_OBJ_SET;

extern REPLAY_OBJ_SET *replaySet;

typedef struct BIRD_OBJ_SET {
	OBJ_ATTR* beak;
	OBJ_ATTR* birdTorso;
	BOOL flap;
} BIRD_OBJ_SET, BIRD_OBJ_SET;

extern BIRD_OBJ_SET *birdSet0;
extern BIRD_OBJ_SET *birdSet1;

//Used for both score and hiscore
typedef struct NUM_OBJ_SET {
	OBJ_ATTR* num0;
	OBJ_ATTR* num1;
	OBJ_ATTR* num2;
	OBJ_ATTR* num3;
	OBJ_ATTR* num4;
} NUM_OBJ_SET, NUM_OBJ_SET;

extern NUM_OBJ_SET *hiScoreSet;
extern NUM_OBJ_SET *scoreSet;

typedef struct HI_OBJ_SET {
	OBJ_ATTR* hi0;
	OBJ_ATTR* hi1;
} HI_OBJ_SET, HI_OBJ_SET;

extern HI_OBJ_SET *hiSet;

typedef struct DINO_OBJ_SET {
	OBJ_ATTR* dinoTorso;
	OBJ_ATTR* dinoTail;
	OBJ_ATTR* dinoLegs;
	OBJ_ATTR* dinoWink;
} DINO_OBJ_SET, DINO_OBJ_SET;

extern DINO_OBJ_SET *dinoSet;

typedef struct CLOUD_OBJ_SET {
	OBJ_ATTR* cloudL;
	OBJ_ATTR* cloudR;
} CLOUD_OBJ_SET, CLOUD_OBJ_SET;

extern CLOUD_OBJ_SET *cloudSet0;
extern CLOUD_OBJ_SET *cloudSet1;
extern CLOUD_OBJ_SET *cloudSet2;
extern CLOUD_OBJ_SET *cloudSet3;
extern CLOUD_OBJ_SET *cloudSet4;
extern CLOUD_OBJ_SET *cloudSet5;

typedef struct MOON_OBJ_SET {
	OBJ_ATTR* moonChunk0;
	OBJ_ATTR* moonChunk1;
	OBJ_ATTR* moonChunk2;
	OBJ_ATTR* moonChunk3;
} MOON_OBJ_SET, MOON_OBJ_SET;

extern MOON_OBJ_SET *moonSet;

typedef struct STAR_OBJ_SET {
	OBJ_ATTR* star;
} STAR_OBJ_SET, STAR_OBJ_SET;

extern STAR_OBJ_SET *starSet0;
extern STAR_OBJ_SET *starSet1;

typedef struct OBSTACLE_OBJ_SET {
	OBJ_ATTR* obstacleChunk0;
	OBJ_ATTR* obstacleChunk1;
	OBJ_ATTR* obstacleChunk2;
	OBJ_ATTR* obstacleChunk3;
} OBSTACLE_OBJ_SET, OBSTACLE_OBJ_SET;

extern OBSTACLE_OBJ_SET *obstacleSet0;
extern OBSTACLE_OBJ_SET *obstacleSet1;

typedef struct GAMEOVER_OBJ_SET {
	OBJ_ATTR* g;
	OBJ_ATTR* a;
	OBJ_ATTR* m;
	OBJ_ATTR* e;
	OBJ_ATTR* o;
	OBJ_ATTR* v;
	OBJ_ATTR* r;
} GAMEOVER_OBJ_SET, GAMEOVER_OBJ_SET;

extern GAMEOVER_OBJ_SET *gameoverSet;

//Metasprite struct manipulators
REPLAY_OBJ_SET *createReplaySet(OBJ_ATTR *obj, OBJ_ATTR *obj2);
void toggleReplayHide(REPLAY_OBJ_SET *set);
void setReplayPos(REPLAY_OBJ_SET *set, int x, int y);

BIRD_OBJ_SET *createBirdSet(OBJ_ATTR *obj, OBJ_ATTR *obj2);
void setBirdPos(BIRD_OBJ_SET *set, int x, int y);
void toggleBirdFlap(BIRD_OBJ_SET *set);

NUM_OBJ_SET *createNumSet(OBJ_ATTR *index);
void setNumPos(NUM_OBJ_SET *set, int x, int y);
void setNumValue(NUM_OBJ_SET *set, int num);

HI_OBJ_SET *createHiSet(OBJ_ATTR *obj0, OBJ_ATTR *obj1);
void setHiPos(HI_OBJ_SET *set, int x, int y);

DINO_OBJ_SET *createDinoSet(OBJ_ATTR *index);
void setDinoPos(DINO_OBJ_SET *set, int x, int y);
void dinoGraphicsUpdate(DINO_OBJ_SET *set);
void setDinoUpright(DINO_OBJ_SET *set);
void setDinoDucking(DINO_OBJ_SET *set);
void setDinoCrashed(DINO_OBJ_SET *set);


void initSets();
void assembleSets();

//Background helpers
void whiteOutBG();
void backgroundInit();


#endif