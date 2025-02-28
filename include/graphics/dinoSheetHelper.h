#ifndef DINO_SHEET_HELPER_H
#define DINO_SHEET_HELPER_H

#include <stdlib.h>
#include "tonc.h"
#include "dino.h"

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

void initSets();
void assembleSets();

//Background helpers
void whiteOutBG();
void backgroundInit();
void updateHorizonTile(int bg_index, int terrainIndex, bool bumpy);
void inversionUpdate(bool night, bool invertOver);
PALETTE_TRACKER *createTrackers();

#endif