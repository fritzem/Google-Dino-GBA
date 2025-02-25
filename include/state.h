#ifndef DINO_GBA_STATE_H
#define DINO_GBA_STATE_H

typedef struct GAME_STATE {
    int speed;
    int curtainScroll;

    int distanceRan;
    int distanceRanPoint;

    int runningFrames;
    int randoFrames;
    bool spawnObstacles;

    bool playing;
    bool playingIntro;

    int gameoverFrames;
} GAME_STATE, GAME_STATE;

#endif //DINO_GBA_STATE_H