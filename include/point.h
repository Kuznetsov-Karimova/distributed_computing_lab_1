#ifndef POINT_H
#define POINT_H

#include <math.h>

typedef struct {
    float x;
    float y;
} Point;

float distFromCenter(Point* point) {
    return sqrtf(powf(point->x, 2) + powf(point->y, 2));
};

#endif