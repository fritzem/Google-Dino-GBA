#include <stdlib.h>
#include "graphics/dinoSheetHelper.h"
#include "dino.h"
#include "tonc.h"
#include "horizon.h"

PALETTE_TRACKER *trackers;

#define PAL_BG    0
#define PAL_OBJ0  0
#define PAL_OBJ1  0
#define PAL_TITLE 1

#define INVERT_COUNT 10

void initSets() {
	trackers = createTrackers();
}

//Terrain helpers

void whiteOutBG() {
	se_fill(&se_mem[29][0], blankTile_SI | SE_PALBANK(PAL_BG));
    se_fill(&se_mem[30][0], transparent_SI | SE_PALBANK(PAL_BG));
    se_fill(&se_mem[31][0], blankTile_SI | SE_PALBANK(PAL_BG));
}

//x1819
void backgroundInit() {
	REG_BG1HOFS = STARTING_CURTAIN_SCROLL;

	for (int i = 0; i < 31; i++) {
		se_plot(&se_mem[31][0], i, GROUND_Y, (firstTerrain_SI + i) | SE_PALBANK(PAL_BG));
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

PALETTE_TRACKER *createTrackers() {
	PALETTE_TRACKER *trackers = malloc(INVERT_COUNT * sizeof(PALETTE_TRACKER));
	for (u16 palIndex = 1; palIndex < INVERT_COUNT; palIndex++) {
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
		for (u16 palIndex = 1; palIndex < INVERT_COUNT; palIndex++) {
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
		for (u16 palIndex = 1; palIndex < INVERT_COUNT; palIndex++) {
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
		for (u16 palIndex = 1; palIndex < INVERT_COUNT; palIndex++) {
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