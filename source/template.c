#include <string.h>
#include "tonc.h"
#include "maro.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

//testing ground
int main()
{
	 memcpy(&tile_mem[4][0], maroTiles, maroTilesLen);
    memcpy(pal_obj_mem, maroPal, maroPalLen);

	 oam_init(obj_buffer, 128);
    REG_DISPCNT= DCNT_OBJ | DCNT_OBJ_1D;

    OBJ_ATTR *maro = &obj_buffer[0];

    obj_set_attr(maro, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(0));

    while(1) {
    	vid_vsync();


    	key_poll();
    	if (key_is_down(KEY_A))
    	{
    		
    	}
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