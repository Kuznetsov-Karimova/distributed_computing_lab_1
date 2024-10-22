#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "point.h"
#include "timer.h"
#ifndef DEPTH
#define DEPTH 1000
#endif

int ntrials;

struct Data{
    unsigned int seed;
    FILE *file;
    pthread_mutex_t *file_mutex;
};

void nextPoint(struct Point* point, struct Point* startPoint) {
    float nextX = powf(point->x, 2) - powf(point->y, 2) + startPoint->x;
    float nextY = point->x * point->y + startPoint->y;
    point->x = nextX;
    point->y = nextY;
};

int isMandel(struct Point* point) {
    struct Point startPoint = {point->x, point->y}; 
    for (int iter = 0; iter < DEPTH; ++iter) {
        if (distFromCenter(point) < 2) {
            nextPoint(point, &startPoint);
        } else {
            return 0;
        }
    }
    return 1;
};

void* mandel(void* data) {
    struct Data* local_data = (struct Data*)data;
    while (ntrials) {
        struct Point point;
        point.x = -2+4*((float)rand_r(&local_data->seed))/RAND_MAX;
        point.y = -2+4*((float)rand_r(&local_data->seed))/RAND_MAX;
        if (isMandel(&point)) {
            pthread_mutex_lock(local_data->file_mutex);
            if (ntrials > 0) {
                --ntrials;
                fprintf(local_data->file, "%lf,%lf\n", point.x, point.y);
            }
            pthread_mutex_unlock(local_data->file_mutex);;
        }
    }
}
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
    struct Data* data = malloc(nthreads * sizeof(struct Data));

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

    return 0;
}