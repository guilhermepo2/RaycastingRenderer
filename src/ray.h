#ifndef RAY_H
#define RAY_H

typedef struct Ray {
    float rayAngle;
    float wallHitX;
    float wallHitY;
    float distance;
    int wasHitVertical;

    int isRayFacingUp;
    int isRayFacingDown;
    int isRayFacingRight;
    int isRayFacingLeft;

    int wallHitContent;
} Ray;

#endif