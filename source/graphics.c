#include <memory.h>
#include <dino_sheet.h>
#include <palette_bank.h>
#include "graphics.h"
#include "tonc.h"
#include "dinoSheetHelper.h"
#include "game.h"


void initGraphicsMem();

void initGraphics() {
    initGraphicsMem();

    whiteOutBG();
    backgroundInit();

    //horizon layer
    REG_BG0CNT = BG_PRIO(3) | BG_CBB(0) | BG_SBB(31) | BG_4BPP | BG_REG_32x32;
    //curtain layer
    REG_BG1CNT = BG_PRIO(0) | BG_CBB(0) | BG_SBB(29) | BG_4BPP | BG_REG_64x32;

    oam_init(obj_buffer, 128);
    initSets();
    assembleSets();

    REG_DISPCNT= DCNT_OBJ | DCNT_OBJ_2D | DCNT_BG0 | DCNT_BG1;
    REG_BLDCNT = BLD_BUILD(BLD_OBJ, BLD_BG0, BLD_STD);
    REG_BLDALPHA = BLDA_BUILD(0, 16);
    REG_BLDY= BLDY_BUILD(16);
}

enum DIR { DOWN, LEFT, RIGHT, UP };
#define KEYMAP(key, mapping) (!!key_is_down(key)) * mapping
const uint8_t *PALETTE_LUT[4][3] = {
        [DOWN] = {GBC_DOWNPal, GBC_ADOWNPal, GBC_BDOWNPal},
        [LEFT] = {GBC_LEFTPal, GBC_ALEFTPal, GBC_BLEFTPal},
        [RIGHT] = {GBC_RIGHTPal, GBC_ARIGHTPal, GBC_BRIGHTPal},
        [UP] = {GBC_UPPal, GBC_AUPPal, GBC_BUPPal}

};

void initGraphicsMem() {
    memcpy(&tile_mem[4][0], dinoSheetTiles, dinoSheetTilesLen);
    memcpy(&tile_mem[0][0], dinoSheetTiles, dinoSheetTilesLen);

    if (key_is_down(KEY_FIRE | KEY_DIR)) {
        int dirIndex = max(
                max(KEYMAP(KEY_DOWN, DOWN), KEYMAP(KEY_UP, UP)),
                max(KEYMAP(KEY_LEFT, LEFT), KEYMAP(KEY_RIGHT, RIGHT))
        );
        int faceIndex = max(key_is_down(KEY_A), key_is_down(KEY_B));
        memcpy(pal_obj_mem, PALETTE_LUT[dirIndex][faceIndex], defaultPalLen);
        memcpy(pal_bg_mem, PALETTE_LUT[dirIndex][faceIndex], defaultPalLen);
    } else {
        memcpy(pal_obj_mem, defaultPal, defaultPalLen);
        memcpy(pal_bg_mem, defaultPal, defaultPalLen);
    }

}