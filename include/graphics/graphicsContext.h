#ifndef DINO_GBA_GFX_CONTEXT_H
#define DINO_GBA_GFX_CONTEXT_H

#include "tonc.h"

typedef struct {
    struct {
        u32 x;
        u32 y;
        u32 width;
        u32 height;
    } viewport;
    OBJ_ATTR * baseOAM;
    OBJ_ATTR * cursorOAM;
} GRAPHICS_CTX;
#define CTX_X ctx->viewport.x
#define CTX_Y ctx->viewport.y
#define CTX_W ctx->viewport.width
#define CTX_H ctx->viewport.height

#define CTX_OAM ctx->cursorOAM

INLINE void INIT_GRAPHICS_CTX_NORMAL(GRAPHICS_CTX * ctx) {
    CTX_X = 0;
    CTX_Y = 0;
    CTX_W = SCREEN_WIDTH;
    CTX_H = SCREEN_HEIGHT;
}

INLINE void INIT_GRAPHICS_CTX_HALF(GRAPHICS_CTX * ctx) {
    CTX_X = 0;
    CTX_Y = 0;
    CTX_W = SCREEN_WIDTH;
    CTX_H = SCREEN_HEIGHT / 2;
}

#endif //DINO_GBA_GFX_CONTEXT_H