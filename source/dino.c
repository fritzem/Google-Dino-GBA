#include <string.h>
#include "tonc.h"
#include "dino.h"
#include "dinoSheet.h"
#include "dinoSheetHelper.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

GAME_STATE *gameState;
DINO_STATE *dinoState;

int main()
{	
	init();

  u32 x = 100;
  u16 counter = 0;

  while(1) {
  	vid_vsync();

    counter++;
  	
    input();
  	REG_BG0HOFS = x - 100;

      if (counter == 30) 
      {
          toggleBirdFlap(birdSet0);
          toggleBirdFlap(birdSet1);
          counter = 0;
      }

      setNumValue(scoreSet, x);

  	oam_copy(oam_mem, obj_buffer, 64);
  }
  return 0;
}

void input() {
	key_poll();
  	if (key_hit(KEY_A))
  	{
  		//x += 1;
  		toggleReplayHide(replaySet);
  	}
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

  REG_BG0CNT = BG_CBB(0) | BG_SBB(31) | BG_4BPP | BG_REG_32x32;

	oam_init(obj_buffer, 128);
	initSets();
	assembleSets();

  REG_DISPCNT= DCNT_OBJ | DCNT_OBJ_2D | DCNT_BG0;
}

void initGame() {
	gameState = malloc(sizeof(GAME_STATE));
	//initState(gameState);
	dinoState = malloc(sizeof(DINO_STATE));
	initDino(dinoState);
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