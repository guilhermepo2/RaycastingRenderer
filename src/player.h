#ifndef PLAYER_H
#define PLAYER_H
#include "mathaux.h"

typedef struct Player {
    Point position;
    Point size;
    int turnDirection; // -1 for left, 1 for right
    int walkDirection; // -1 for back, 1 for front
    float rotationAngle;
    float walkSpeed;
    float turnSpeed;
} Player;

int SetPlayerPosition(Player* player, float x, float y) {
    if(player == NULL) {
        return FALSE;
    }

    player->position.x = x;
    player->position.y = y;
    return TRUE;
}

int SetPlayerSize(Player* player, float x, float y) {
    if(player == NULL) {
        return FALSE;
    }

    player->size.x = x;
    player->size.y = y;

    return TRUE;
}

#endif