#ifndef PLAYER_H
#define PLAYER_H

typedef struct Player {
    float x;
    float y;
    float width;
    float height;

    int turnDirection; // -1 for left, 1 for right
    int walkDirection; // -1 for back, 1 for front

    float rotationAngle;
    float walkSpeed;
    float turnSpeed;
} Player;

#endif