#include <string.h>
#include "tonc.h"
#include "maro.h"
#include "dinoSheet.h"
#include "dinoSheetHelper.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

//testing ground
int main()
{
	 memcpy(&tile_mem[4][0], dinoSheetTiles, dinoSheetTilesLen);
    memcpy(pal_obj_mem, dinoSheetPal, dinoSheetPalLen);

	 oam_init(obj_buffer, 128);
    REG_DISPCNT= DCNT_OBJ | DCNT_OBJ_2D;

    OBJ_ATTR *dinoHead = &obj_buffer[0];
    

    REPLAY_OBJ_SET *replaySet = createReplaySet(&obj_buffer[1],&obj_buffer[2]);

    obj_set_attr(dinoHead, ATTR0_SQUARE, ATTR1_SIZE_32,  dinoHeadSI | ATTR2_PALBANK(0));
	 //obj_set_attr(maro2, ATTR0_SQUARE, ATTR1_HFLIP | ATTR1_SIZE_8, ATTR2_PALBANK(0));

    u32 x = 100, y = 50;
    u32 metaX = 75, metaY = 100;
    obj_set_pos(dinoHead, x, y);
    setReplayPos(replaySet, metaX, metaY);
    //obj_set_pos(maro2, y, x);

    while(1) {
    	vid_vsync();


    	key_poll();
    	if (key_is_down(KEY_A))
    	{
    		x += 1;
    		metaX += 1;
    	}

    	obj_set_pos(dinoHead, x / 2, y);
    	setReplayPos(replaySet, metaX / 2, metaY);
    	oam_copy(oam_mem, obj_buffer, 3);
    }
    return 0;
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