#include "hitbox.h"

void adjustBox(COLLISION_BOX * box, int size, int width) {
    if (size > 1) {
        (box + 1)->w = width - box->w - (box + 2)->w;
        (box + 2)->x = width - (box + 2)->w;
    }
}

void cloneBox(COLLISION_BOX * clone, const COLLISION_BOX * base, int boxes) {
    for (int i = 0; i < boxes; i++) {
        (clone + i)->x = (base + i)->x;
        (clone + i)->y = (base + i)->y;
        (clone + i)->w = (base + i)->w;
        (clone + i)->h = (base + i)->h;
    }
}

bool boxCheck(COLLISION_BOX * a, COLLISION_BOX * b) {
    return 	(a->x < b->x + b->w &&
               a->x + a->w > b->x &&
               a->y < b->y + b->h &&
               a->y + a->h > b->y);
}

bool boxCheckOffset(const COLLISION_BOX * a, COLLISION_BOX * b,
                    int x1, int y1, int x2, int y2) {
    return 	(a->x + x1 < b->x + x2 + b->w &&
               a->x + x1 + a->w > b->x + x2 &&
               a->y + y1 < b->y + y2 + b->h &&
               a->y + y1 + a->h > b->y + y2);
}
