#ifndef DINO_SHEET_HELPER_H
#define DINO_SHEET_HELPER_H

#include <stdlib.h>
#include "tonc.h"

#define attr0Visibility 0x200

//Sprite Indices
#define replaySI 0x0
#define replayTailSI 0x0

#define cloudLeftSI 0x4
#define cloudRightSI 0x8

#define smallCactusSingleSI 0xA
#define smallCactusDoubleSI 0xA
#define smallCactusTripleSI 0x11
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

#define star0_SI 0x46
#define star1_SI 0x48
#define star2_SI 0x88
INLINE int starSI(int x) {return (x == 0) ? star0_SI : (x == 1) ? star1_SI : star2_SI;}

#define fullMoonTopSI 0xE8
#define moon0_SI 0x100
#define moon1_SI 0x104
#define moon2_SI 0x108
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

#define charY 40
#define charXg 20
#define charXa charXg + 24
#define charXm charXa + 24
#define charXe charXm + 24
#define charXo charXe + 44
#define charXv charXo + 24
#define charXe2 charXv + 24
#define charXr charXe2 + 24

#define hiSI 0xDA

#define titleLeftSI 0x380
#define titleRightSI 0x388

//Terrain 

#define blankTile_SI 0x359
#define transparent_SI 0xA
#define firstTerrain_SI 0x2C0

#define GROUND_Y 18
#define TERRAIN_STRIP_LENGTH 75
#define BUMPY_OFFSET 96
#define BUMPY_TOP_OFFSET 64

#define TILE_SIZE 8
#define BG_TILE_LENGTH 32

#define CLOUD_WIDTH 48

#define PALETTE_POINT 1000

#define TYPEM 10
#define CACTUS_SMALL_0 1
#define CACTUS_SMALL_1 2
#define CACTUS_SMALL_2 3
#define CACTUS_LARGE_0 11
#define CACTUS_LARGE_1 12
#define CACTUS_LARGE_2 13
#define PTERODACTYL_0 20
#define PTERODACTYL_1 21
#define PTERODACTYL_2 22

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

extern CLOUD_OBJ_SET * clouds;

typedef struct MOON_OBJ_SET {
	OBJ_ATTR* moonChunk0;
	OBJ_ATTR* moonChunk1;
	OBJ_ATTR* moonChunk2;
	OBJ_ATTR* moonChunk3;
} MOON_OBJ_SET, MOON_OBJ_SET;

extern MOON_OBJ_SET *moonSet;

typedef struct STARS_OBJ_SET {
	OBJ_ATTR* star0;
	OBJ_ATTR* star1;
} STARS_OBJ_SET, STARS_OBJ_SET;

extern STARS_OBJ_SET *starsSet;

typedef struct OBSTACLE_OBJ_SET {
	OBJ_ATTR* obstacleChunk0;
	OBJ_ATTR* obstacleChunk1;
	OBJ_ATTR* obstacleChunk2;
	OBJ_ATTR* obstacleChunk3;
} OBSTACLE_OBJ_SET, OBSTACLE_OBJ_SET;

extern OBSTACLE_OBJ_SET *obstacleSets;

typedef struct GAMEOVER_OBJ_SET {
	OBJ_ATTR* g;
	OBJ_ATTR* a;
	OBJ_ATTR* m;
	OBJ_ATTR* e;
	OBJ_ATTR* o;
	OBJ_ATTR* v;
	OBJ_ATTR* e2;
	OBJ_ATTR* r;
} GAMEOVER_OBJ_SET, GAMEOVER_OBJ_SET;

typedef struct TITLE_OBJ_SET {
	OBJ_ATTR* left;
	OBJ_ATTR* right;
} TITLE_OBJ_SET, TITLE_OBJ_SET;

extern TITLE_OBJ_SET *titleSet;

extern GAMEOVER_OBJ_SET *gameoverSet;

typedef struct PALETTE_TRACKER {
	u8 minRed;
	u8 maxRed;
	int curRed;
	int incRed;

	u8 minGreen;
	u8 maxGreen;
	int curGreen;
	int incGreen;

	u8 minBlue;
	u8 maxBlue;
	int curBlue;
	int incBlue;
} PALETTE_TRACKER, PALETTE_TRACKER;

extern PALETTE_TRACKER *paletteTrackers;

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
void hideNum(NUM_OBJ_SET *set);
void showNum(NUM_OBJ_SET *set);

HI_OBJ_SET *createHiSet(OBJ_ATTR *obj0, OBJ_ATTR *obj1);
void setHiPos(HI_OBJ_SET *set, int x, int y);

DINO_OBJ_SET *createDinoSet(OBJ_ATTR *index);
void setDinoPos(DINO_OBJ_SET *set, int x, int y);
void dinoGraphicsUpdate(DINO_OBJ_SET *set);
void setDinoAnim(DINO_OBJ_SET *set, int frame);
void setDinoUpright(DINO_OBJ_SET *set);
void setDinoDucking(DINO_OBJ_SET *set);
void setDinoCrashed(DINO_OBJ_SET *set);
void dinoBlink(DINO_OBJ_SET *set);
void dinoUnBlink(DINO_OBJ_SET *set);

CLOUD_OBJ_SET *createCloudSets(OBJ_ATTR *index);
void setCloudPos(CLOUD_OBJ_SET *set, int x, int y);

STARS_OBJ_SET *createStarsSet(OBJ_ATTR *index);
void setStarTypes(STARS_OBJ_SET *set, int star0, int star1);
void setStarPos(STARS_OBJ_SET *set, int x0, int y0, int x1, int y1);

MOON_OBJ_SET *createMoonSet(OBJ_ATTR *index);
int incrementMoonPhase(MOON_OBJ_SET *set, int currentPhase);
void setMoonPos(MOON_OBJ_SET *set, int x, int y);

OBSTACLE_OBJ_SET *createObstacleSet(OBJ_ATTR *index);
void wipeObstacleSet(OBSTACLE_OBJ_SET *set);
void setObstacleSet(OBSTACLE_OBJ_SET *set, int type, int size);
void setObstaclePos(OBSTACLE_OBJ_SET *set, int type, int size, int x, int y);
bool toggleDactylFlap(OBSTACLE_OBJ_SET *set, bool flap);

GAMEOVER_OBJ_SET *createGameoverSet(OBJ_ATTR *index);
void showGameover(GAMEOVER_OBJ_SET * set);
void hideGameover(GAMEOVER_OBJ_SET * set);

TITLE_OBJ_SET *createTitleSet(OBJ_ATTR *index);
void hideTitle(TITLE_OBJ_SET * set);

void initSets();
void assembleSets();

//Background helpers
void whiteOutBG();
void backgroundInit();
void updateHorizonTile(int bg_index, int terrainIndex, bool bumpy);
void invertPalettes();
void inversionUpdate(bool night, bool invertOver);
PALETTE_TRACKER *createTrackers();

#endif