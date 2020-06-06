#ifndef MATH_AUX_H
#define MATH_AUX_H

float DistanceBetweenPoints(float x1, float y1, float x2, float y2) {
    return sqrt( ((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)) );
}

float NormalizeAngle(float angle) {
    angle = remainder(angle, TWO_PI);
    
    if(angle < 0) {
        angle = TWO_PI + angle;
    }

    return angle;
}

#endif