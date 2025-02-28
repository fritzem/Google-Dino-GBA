#include <memory.h>
#include <dino_sheet.h>
#include <palette_bank.h>
#include "graphics.h"
#include "tonc.h"
#include "dinoSheetHelper.h"
#include "game.h"

const int animRun[] = {dinoFeet1_SI, dinoFeet2_SI};
const int animDuc[] = {dinoCrouchFeet0_SI, dinoCrouchFeet1_SI};

#define PAL_BG    0
#define PAL_OBJ0  0
#define PAL_OBJ1  0
#define PAL_TITLE 1

#define SCORE_Y 10
#define HI_X 74
#define HISCORE_X 107
#define SCORE_X 174

OBJ_ATTR obj_buffer[128];

void initGraphicsMem();

OBJ_ATTR * drawTitle(OBJ_ATTR * obj) {
    obj_set_attr(obj++, Y(40) | ATTR0_WIDE,
                ATTR1_SIZE_64,  titleLeftSI | ATTR2_PALBANK(PAL_TITLE));
    obj_set_attr(obj++, Y(40) | ATTR0_WIDE,
                  64 | ATTR1_SIZE_64, titleRightSI | ATTR2_PALBANK(PAL_TITLE));
    return obj;
}

OBJ_ATTR * drawReplay(OBJ_ATTR * obj) {
        obj_set_attr(obj++, ATTR0_SQUARE | Y(70),
                     ATTR1_SIZE_32 | X(102),
                     replaySI | ATTR2_PALBANK(PAL_OBJ0));
        obj_set_attr(obj++, ATTR0_TALL | Y(70),
                     ATTR1_SIZE_16 | ATTR1_HFLIP | X(102 + 32),
                     replayTailSI | ATTR2_PALBANK(PAL_OBJ0));
    return obj;
}

#define GOVR_CHAR(si, x) \
obj_set_attr(obj++, Y(charY) | ATTR0_SQUARE, \
    X(x) | ATTR1_SIZE_16, \
    si | ATTR2_PALBANK(PAL_OBJ0))
OBJ_ATTR * drawGameover(OBJ_ATTR * obj) {
    GOVR_CHAR(charG_SI, charXg);
    GOVR_CHAR(charA_SI, charXa);
    GOVR_CHAR(charM_SI, charXm);
    GOVR_CHAR(charE_SI, charXe);
    GOVR_CHAR(charO_SI, charXo);
    GOVR_CHAR(charV_SI, charXv);
    GOVR_CHAR(charE_SI, charXe2);
    GOVR_CHAR(charR_SI, charXr);
    return obj;
}

OBJ_ATTR * drawHi(OBJ_ATTR * obj) {
    obj_set_attr(obj++, ATTR0_SQUARE | Y(SCORE_Y),
                 ATTR1_SIZE_16 | X(HI_X),
                 hiSI | ATTR2_PALBANK(PAL_OBJ0) | ATTR2_PRIO(1));
    obj_set_attr(obj++, ATTR0_TALL | Y(SCORE_Y),
                 ATTR1_SIZE_8 | X(HI_X + 16),
                 (hiSI + 2) | ATTR2_PALBANK(PAL_OBJ0) | ATTR2_PRIO(1));
    return obj;
}

OBJ_ATTR * drawNumber(OBJ_ATTR * obj, u32 posX, u32 number) {
    u32 digit = numToSI(number % 10);
    obj_set_attr(obj++, ATTR0_SQUARE | Y(SCORE_Y),
                 ATTR1_SIZE_16 | X(posX + 44),
                 digit | ATTR2_PALBANK(PAL_OBJ0) | ATTR2_PRIO(1));
    number /= 10;
    digit = numToSI(number % 10);
    obj_set_attr(obj++, ATTR0_SQUARE | Y(SCORE_Y),
                 ATTR1_SIZE_16 | X(posX + 33),
                 digit | ATTR2_PALBANK(PAL_OBJ0) | ATTR2_PRIO(1));
    number /= 10;
    digit = numToSI(number % 10);
    obj_set_attr(obj++, ATTR0_SQUARE | Y(SCORE_Y),
                 ATTR1_SIZE_16 | X(posX + 22),
                 digit | ATTR2_PALBANK(PAL_OBJ0) | ATTR2_PRIO(1));
    number /= 10;
    digit = numToSI(number % 10);
    obj_set_attr(obj++, ATTR0_SQUARE | Y(SCORE_Y),
                 ATTR1_SIZE_16 | X(posX + 11),
                 digit | ATTR2_PALBANK(PAL_OBJ0) | ATTR2_PRIO(1));
    number /= 10;
    digit = numToSI(number % 10);
    obj_set_attr(obj++, ATTR0_SQUARE | Y(SCORE_Y),
                 ATTR1_SIZE_16 | X(posX),
                 digit | ATTR2_PALBANK(PAL_OBJ0) | ATTR2_PRIO(1));
    return obj;
}

OBJ_ATTR * drawDino(OBJ_ATTR * obj, DINO_STATE * dinoState) {
    int x = dinoState->xPos;
    int y = SCREEN_HEIGHT - 48 - dinoState->yPos;
    int crouchY = SCREEN_HEIGHT - 32 - dinoState->yPos;
    switch (dinoState->status) {
        case CRASHED:
            obj_set_attr(obj++, ATTR0_SQUARE | Y(y),
                         ATTR1_SIZE_64 | X(x - 22),
                         deadDinoSI | ATTR2_PALBANK(PAL_OBJ0));
            break;
        case DUCKING:
            obj_set_attr(obj++, ATTR0_WIDE | Y(crouchY),
                         ATTR1_SIZE_64 | X(x),
                         dinoCrouchSI | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_WIDE | Y(crouchY + 16),
                         ATTR1_SIZE_32 | X(x + 8),
                         animDuc[dinoState->frame] | ATTR2_PALBANK(PAL_OBJ0));
            break;
        case RUNNING:
            obj_set_attr(obj++, ATTR0_SQUARE | Y(y),
                         ATTR1_SIZE_32 | X(x + 16),
                         dinoHeadSI | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_SQUARE | Y(y + 16),
                         ATTR1_SIZE_16 | X(x),
                         dinoTailSI | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_WIDE | Y(y + 32),
                         ATTR1_SIZE_32 | X(x),
                         animRun[dinoState->frame] | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_HIDE | Y(y),
                         ATTR1_SIZE_8 | X(x + 24),
                         dinoWinkSI | ATTR2_PALBANK(PAL_OBJ0));
            break;
        case WAITING:
        case JUMPING:
            obj_set_attr(obj++, ATTR0_SQUARE | (ATTR0_HIDE * (!dinoState->blinking)) | Y(y),
                         ATTR1_SIZE_8 | X(x + 24),
                         dinoWinkSI | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_SQUARE | Y(y),
                         ATTR1_SIZE_32 | X(x + 16),
                         dinoHeadSI | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_SQUARE | Y(y + 16),
                         ATTR1_SIZE_16 | X(x),
                         dinoTailSI | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_WIDE | Y(y + 32),
                         ATTR1_SIZE_32 | X(x),
                         dinoFeet0_SI | ATTR2_PALBANK(PAL_OBJ0));
            break;
    }

    return obj;
}

#define TYPEM 10
#define CACTUS_SMALL_SIZE(s) (CACTUS_SMALL * TYPEM + s)
#define CACTUS_LARGE_SIZE(s) (CACTUS_LARGE * TYPEM + s)
#define PTERODACTYL_FLAP(flap) (PTERODACTYL * TYPEM + flap)

OBJ_ATTR * drawObstacle(OBJ_ATTR * obj, OBSTACLE * obstacle) {
    s32 x = obstacle->x;
    s32 y = obstacle->y;

    switch (obstacle->type * TYPEM + obstacle->typeCategory) {
        case CACTUS_SMALL_SIZE(1):
            obj_set_attr(obj++, ATTR0_SQUARE | Y(CACTUS_SMALL_Y),
                         ATTR1_SIZE_32 | X(x - 15),
                         smallCactusSingleSI | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_SQUARE | Y(CACTUS_SMALL_Y + 32),
                         ATTR1_SIZE_8 | X(x + 5),
                         smallCactusStumpSI | ATTR2_PALBANK(PAL_OBJ0));
            break;
        case CACTUS_SMALL_SIZE(2):
            obj_set_attr(obj++, ATTR0_WIDE | Y(CACTUS_SMALL_Y),
                         ATTR1_SIZE_64 | X(x - 15),
                         smallCactusDoubleSI | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_SQUARE | Y(CACTUS_SMALL_Y + 32),
                         ATTR1_SIZE_8 | X(x + 5),
                         smallCactusStumpSI | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_SQUARE | Y(CACTUS_SMALL_Y + 32),
                         ATTR1_SIZE_8 | X(x + 22),
                         smallCactusStumpSI | ATTR2_PALBANK(PAL_OBJ0));
            break;
        case CACTUS_SMALL_SIZE(3):
            obj_set_attr(obj++, ATTR0_WIDE | Y(CACTUS_SMALL_Y),
                         ATTR1_SIZE_64 | X(x - 8),
                         smallCactusTripleSI | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_SQUARE | Y(CACTUS_SMALL_Y + 32),
                         ATTR1_SIZE_8 | X(x + 5),
                         smallCactusStumpSI | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_SQUARE | Y(CACTUS_SMALL_Y + 32),
                         ATTR1_SIZE_8 | X(x + 22),
                         smallCactusStumpSI | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_SQUARE | Y(CACTUS_SMALL_Y + 32),
                         ATTR1_SIZE_8 | X(x + 39),
                         smallCactusStumpSI | ATTR2_PALBANK(PAL_OBJ0));
            break;
        case CACTUS_LARGE_SIZE(1):
            obj_set_attr(obj++, ATTR0_TALL | Y(CACTUS_LARGE_Y),
                         ATTR1_SIZE_64 | X(x),
                         bigCactusSingleSI | ATTR2_PALBANK(PAL_OBJ0));
            break;
        case CACTUS_LARGE_SIZE(2):
            obj_set_attr(obj++, ATTR0_SQUARE | Y(CACTUS_LARGE_Y),
                         ATTR1_SIZE_64 | X(x),
                         bigCactusDoubleSI | ATTR2_PALBANK(PAL_OBJ0));
            break;
        case CACTUS_LARGE_SIZE(3):
            obj_set_attr(obj++, ATTR0_SQUARE | Y(CACTUS_LARGE_Y),
                         ATTR1_SIZE_64 | X(x),
                         bigCactusTripleSI | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_TALL | Y(CACTUS_LARGE_Y),
                         ATTR1_SIZE_64 | X(x + 64),
                         bigCactusTripleRunoffSI | ATTR2_PALBANK(PAL_OBJ0));
            break;
        case PTERODACTYL_FLAP(false):
            obj_set_attr(obj++, ATTR0_SQUARE | Y(y),
                         ATTR1_SIZE_16 | X(x),
                         birdBeakSI | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_SQUARE | Y(y - 4),
                         ATTR1_SIZE_32 | X(x + 16),
                         birdFlapUpSI | ATTR2_PALBANK(PAL_OBJ0));
            break;
        case PTERODACTYL_FLAP(true):
            obj_set_attr(obj++, ATTR0_SQUARE | Y(y),
                         ATTR1_SIZE_16 | X(x),
                         birdBeakSI | ATTR2_PALBANK(PAL_OBJ0));
            obj_set_attr(obj++, ATTR0_SQUARE | Y(y + 4),
                         ATTR1_SIZE_32 | X(x + 16),
                         birdFlapDownSI | ATTR2_PALBANK(PAL_OBJ0));
            break;
    }
    return obj;
}

OBJ_ATTR * drawCloud(OBJ_ATTR * obj, CLOUD * cloud) {
    obj_set_attr(obj++, ATTR0_WIDE | Y(cloud->yPos),
                 ATTR1_SIZE_32 | X(cloud->xPos),
                 cloudLeftSI | ATTR2_PALBANK(PAL_OBJ1));
    obj_set_attr(obj++, ATTR0_SQUARE | Y(cloud->yPos),
                 ATTR1_SIZE_16 | X(cloud->xPos + 32),
                 cloudRightSI | ATTR2_PALBANK(PAL_OBJ1));
    return obj;
}

OBJ_ATTR * drawStars(OBJ_ATTR * obj, HORIZON_STATE * horizonState) {
    obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_BLEND | Y(horizonState->star0Y),
                 ATTR1_SIZE_16 | X(horizonState->star0X),
                 star0_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
    obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_BLEND | Y(horizonState->star1Y),
                 ATTR1_SIZE_16 | X(horizonState->star1X),
                 star1_SI | ATTR2_PRIO(2) |ATTR2_PALBANK(PAL_OBJ1));
    return obj;
}

OBJ_ATTR * drawMoon(OBJ_ATTR * obj, HORIZON_STATE * horizonState) {
    switch(horizonState->moonPhase) {
        case 0:
            obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_BLEND | Y(MOON_Y),
                         ATTR1_SIZE_32 | ATTR1_HFLIP | X(horizonState->moonX),
                         moon1_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_BLEND | Y(MOON_Y + 8),
                         ATTR1_SIZE_32 | ATTR1_HFLIP | ATTR1_VFLIP | X(horizonState->moonX),
                         moon1_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            break;
        case 1:
            obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_BLEND | Y(MOON_Y),
                         ATTR1_SIZE_32 | ATTR1_HFLIP | X(horizonState->moonX),
                         moon2_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_BLEND | Y(MOON_Y + 8),
                         ATTR1_SIZE_32 | ATTR1_HFLIP | ATTR1_VFLIP | X(horizonState->moonX),
                         moon2_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            break;
        case 2:
            obj_set_attr(obj++, ATTR0_TALL | ATTR0_BLEND | Y(MOON_Y),
                         ATTR1_SIZE_32 | X(horizonState->moonX),
                         moon2_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            obj_set_attr(obj++, ATTR0_TALL | ATTR0_BLEND | Y(MOON_Y + 8),
                         ATTR1_SIZE_32 | ATTR1_VFLIP | X(horizonState->moonX),
                         moon2_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            obj_set_attr(obj++, ATTR0_WIDE | ATTR0_BLEND | Y(MOON_Y),
                         ATTR1_SIZE_32 | X(horizonState->moonX + 8),
                         fullMoonTopSI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_BLEND | Y(MOON_Y + 8),
                         ATTR1_SIZE_32 | X(horizonState->moonX + 8),
                         fullMoonSI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            break;
        case 3:
            obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_BLEND | Y(MOON_Y),
                         ATTR1_SIZE_32 | X(horizonState->moonX),
                         moon2_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_BLEND | Y(MOON_Y + 8),
                         ATTR1_SIZE_32 | ATTR1_VFLIP | X(horizonState->moonX),
                         moon2_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            break;
        case 4:
            obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_BLEND | Y(MOON_Y),
                         ATTR1_SIZE_32 | X(horizonState->moonX),
                         moon1_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_BLEND | Y(MOON_Y + 8),
                         ATTR1_SIZE_32 | ATTR1_VFLIP | X(horizonState->moonX),
                         moon1_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            break;
        case 5:
            obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_BLEND | Y(MOON_Y),
                         ATTR1_SIZE_32 | X(horizonState->moonX),
                         moon0_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_BLEND | Y(MOON_Y + 8),
                         ATTR1_SIZE_32 | ATTR1_VFLIP | X(horizonState->moonX),
                         moon0_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            break;
        case 6:
            obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_BLEND | Y(MOON_Y),
                         ATTR1_SIZE_32 | ATTR1_HFLIP | X(horizonState->moonX),
                         moon0_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            obj_set_attr(obj++, ATTR0_SQUARE | ATTR0_BLEND | Y(MOON_Y + 8),
                         ATTR1_SIZE_32 | ATTR1_HFLIP | ATTR1_VFLIP | X(horizonState->moonX),
                         moon0_SI | ATTR2_PRIO(2) | ATTR2_PALBANK(PAL_OBJ1));
            break;
    }
    return obj;
}

void drawGame(GRAPHICS_CTX * ctx, STATE * state) {
    OBJ_ATTR * objCursor = ctx->baseOAM;
    if (state->dinoState.status == WAITING)
        objCursor = drawTitle(objCursor);
    if (state->dinoState.status == CRASHED) {
        objCursor = drawReplay(objCursor);
        objCursor = drawGameover(objCursor);
    }
    objCursor = drawHi(objCursor);
    objCursor = drawNumber(objCursor, HISCORE_X, state->gameState.hiScore);
    if (!state->meterState.flashing)
        objCursor = drawNumber(objCursor, SCORE_X, state->meterState.displayNumber);
    objCursor = drawDino(objCursor, &state->dinoState);
    for (int i = 0; i < state->horizonState.obstacleCount; i++) {
        int obstacle = (1 + i + state->horizonState.obstacleCursor) % MAX_OBSTACLES;
        objCursor = drawObstacle(objCursor, state->horizonState.obstacles + obstacle);
    }
    for (int i = 0; i < state->horizonState.cloudCount; i++) {
        int cloudIndex = (state->horizonState.cloudCursor + i) % MAX_CLOUDS;
        objCursor = drawCloud(objCursor, state->horizonState.clouds + cloudIndex);
    }
    objCursor = drawStars(objCursor, &state->horizonState);
    objCursor = drawMoon(objCursor, &state->horizonState);

    u32 emptyOAM = 64 - (objCursor - ctx->baseOAM);
    oam_init(objCursor, emptyOAM);
    oam_copy(oam_mem, obj_buffer, 64);
}

GRAPHICS_CTX * initGraphics() {
    initGraphicsMem();

    whiteOutBG();
    backgroundInit();

    //horizon layer
    REG_BG0CNT = BG_PRIO(3) | BG_CBB(0) | BG_SBB(31) | BG_4BPP | BG_REG_32x32;
    //curtain layer
    REG_BG1CNT = BG_PRIO(0) | BG_CBB(0) | BG_SBB(29) | BG_4BPP | BG_REG_64x32;

    oam_init(obj_buffer, 128);
    initSets();

    REG_DISPCNT= DCNT_OBJ | DCNT_OBJ_2D | DCNT_BG0 | DCNT_BG1;
    REG_BLDCNT = BLD_BUILD(BLD_OBJ, BLD_BG0, BLD_STD);
    REG_BLDALPHA = BLDA_BUILD(0, 16);
    REG_BLDY= BLDY_BUILD(16);

    GRAPHICS_CTX *ctx = calloc(sizeof(GRAPHICS_CTX), 1);
    ctx->baseOAM = obj_buffer;
    return ctx;
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