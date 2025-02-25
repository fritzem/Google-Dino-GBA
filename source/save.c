#include "save.h"

#define HISCORE (u8 *) MEM_SRAM

s32 readHiscore() {
    int score = 0;
    score += *HISCORE;
    score += *(HISCORE + 1) << 8;
    score += *(HISCORE + 2) << 16;
    score += *(HISCORE + 3) << 24;
    return score;
}

void setHiscore(int score) {
    *HISCORE = score & 0xFF;
    *(HISCORE + 1) = (score >> 8) & 0xFF;
    *(HISCORE + 2) = (score >> 16) & 0xFF;
    *(HISCORE + 3) = (score >> 24) & 0xFF;
}