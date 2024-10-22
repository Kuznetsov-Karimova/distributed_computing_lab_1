#ifndef POINT_H
#define POINT_H

#include <math.h>

struct Point{
    float x;
    float y;
};

float distFromCenter(struct Point* point) {
    return sqrtf(powf(point->x, 2) + powf(point->y, 2));
};

#endif