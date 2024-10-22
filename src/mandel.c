#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "point.h"
#include "timer.h"
#ifndef DEPTH
#define DEPTH 1000
#endif

typedef struct {
    unsigned int seed;
    FILE *file;
    pthread_mutex_t *file_mutex;
} Data;

/* Global variables */

int ntrials;

/* Functions */

void next_point(Point* point, Point* startPoint);

int is_Mandel(Point* point);

void* mandel(void* data);

/*-----------------------------------------------------------------*/

int main(int argc, char *argv[]) {

    if (argc != 3) {
        exit(EXIT_FAILURE);
    }

    int nthreads = atoi(argv[1]);
    ntrials = atoi(argv[2]);

    FILE* file = fopen("mandel.csv", "w");
    pthread_mutex_t file_mutex;
    pthread_mutex_init(&file_mutex, NULL);

    pthread_t* thread_handles = malloc(nthreads * sizeof(pthread_t));
    Data* data = malloc(nthreads * sizeof(Data));

    double start, end;
    GET_TIME(start);

    for (int i = 0; i < nthreads; ++i) {
        data[i].seed = rand();
        data[i].file = file;
        data[i].file_mutex = &file_mutex;
        pthread_create(&thread_handles[i], NULL, mandel, &data[i]);
    }

    for (int i = 0; i < nthreads; ++i) {
        pthread_join(thread_handles[i], NULL);
    }

    pthread_mutex_destroy(&file_mutex);

    GET_TIME(end);

    printf("Time: %f\n", end - start);
    free(thread_handles);
    free(data);
    return 0;
}

/*-----------------------------------------------------------------*/

void next_point(Point* startPoint, Point* point) {
    float nextX = powf(point->x, 2) - powf(point->y, 2) + startPoint->x;
    float nextY = point->x * point->y + startPoint->y;
    point->x = nextX;
    point->y = nextY;
};

int is_Mandel(Point* startPoint) {
    Point point = {startPoint->x, startPoint->y}; 
    for (int iter = 0; iter < DEPTH; ++iter) {
        if (distFromCenter(startPoint) < 2) {
            next_point(startPoint, &point);
        } else {
            return 0;
        }
    }
    return 1;
};

void* mandel(void* data) {
    Data* local_data = (Data*)data;
    while (ntrials) {
        Point point;
        point.x = -2+4*((float)rand_r(&local_data->seed))/RAND_MAX;
        point.y = -2+4*((float)rand_r(&local_data->seed))/RAND_MAX;
        if (is_Mandel(&point)) {
            pthread_mutex_lock(local_data->file_mutex);
            if (ntrials > 0) {
                --ntrials;
                fprintf(local_data->file, "%lf,%lf\n", point.x, point.y);
            }
            pthread_mutex_unlock(local_data->file_mutex);;
        }
    }
    return NULL;
}