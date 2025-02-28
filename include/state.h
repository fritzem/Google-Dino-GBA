#ifndef DINO_GBA_STATE_H
#define DINO_GBA_STATE_H

typedef struct GAME_STATE {
    int speed;
    int curtainScroll;

    int hiScore;

    int distanceRan;
    int distanceRanPoint;

    int runningFrames;
    int randoFrames;
    bool spawnObstacles;

    bool playing;
    bool playingIntro;

    int gameoverFrames;
} GAME_STATE, GAME_STATE;

typedef enum {CRASHED, DUCKING, JUMPING, RUNNING, WAITING} DINO_STATUS;

typedef struct DINO_STATE {
    int xPos;
    int yPos;
    int jumpVelocity;

    DINO_STATUS status;
    bool speedDrop;
    bool jumped;
    bool reachedMin;

    int frame;
    int frameCounter;
    int frameTime;

    int blinkTime;
    int blinks;
    int blinkFrame;
    bool blinking;
} DINO_STATE, DINO_STATE;

typedef struct HORIZON_STATE {
    int scroll;
    int nextScrollTile;
    int scrolled;
    int terrainScroll;
    bool bumpy;
    int extraScroll;

    struct CLOUD * clouds;
    int cloudCursor;
    int cloudCount;

    int invertTimer;
    int invertFrame;
    int fadeFrame;
    int opacity;
    bool night;
    bool inverting;
    bool fading;

    int star0X;
    int star0Y;
    int star1X;
    int star1Y;
    int starMov;

    int moonPhase;
    int moonX;
    int moonMov;

    struct OBSTACLE * obstacles;
    int obstacleCount;
    int obstacleCursor;
    int lastObstacle;
} HORIZON_STATE, HORIZON_STATE;

typedef struct METER_STATE {
    int distance;
    int invertCounter;
    int achievementCounter;

    bool achieving;
    int flashFrame;
    int flashIteration;

    u32 displayNumber;
    bool flashing;
} METER_STATE, METER_STATE;

typedef struct {
    GAME_STATE gameState;
    DINO_STATE dinoState;
    HORIZON_STATE horizonState;
    METER_STATE meterState;
} STATE;

#endif //DINO_GBA_STATE_H