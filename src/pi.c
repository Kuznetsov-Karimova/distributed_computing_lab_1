#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"
#include "point.h"

typedef struct {
    long local_trials;
    unsigned int seed;
    int local_result;
} Data;

/* Global variables */
pthread_t* thread_handles;

/* Functions */

int isInCircle(Point* point);

void* get_PI(void* data);

void init_Data(Data* data, int nthreads, long comm_size);

float count_PI(Data* data, int nthreads, long ntrials);

void print_result(double result, double time);

/*-----------------------------------------------------------------*/

int main(int argc, char *argv[]) {
    if (argc != 3) {
        exit(EXIT_FAILURE);
    }

    int nthreads = atoi(argv[1]);
    long ntrials = atol(argv[2]);
    long comm_size = ntrials / nthreads;


    thread_handles = malloc(nthreads * sizeof(pthread_t));
    Data* data = malloc(nthreads * sizeof(Data));

    double start, end;
    GET_TIME(start);

    init_Data(data, nthreads, comm_size);
    float PI = count_PI(data, nthreads, ntrials);

    GET_TIME(end);

    print_result(PI, end-start);

    free(thread_handles);
    free(data);

    return 0;
}

/*-----------------------------------------------------------------*/
int isInCircle(Point* point) {
    return (distFromCenter(point) <= 1) ? 1 : 0;
}

void* get_PI(void* data) {
    Data* local_data = (Data*)data;
    Point point;

    for (int numPoint = 0; numPoint < local_data->local_trials; ++numPoint) {
        point.x = -1 + 2 * (float) rand_r(&local_data->seed) / RAND_MAX;
        point.y = -1 + 2 * (float) rand_r(&local_data->seed) / RAND_MAX;

        if (isInCircle(&point)) {
           local_data->local_result++;
        }
    }

    return (void*)(&local_data->local_result);
}

void init_Data(Data* data, int nthreads, long comm_size) {
    for (int i = 0; i < nthreads; ++i) {
        data[i].seed = rand();
        data[i].local_trials = comm_size;
        data[i].local_result = 0;
        pthread_create(&thread_handles[i], NULL, get_PI, &data[i]);
    }
}

float count_PI(Data* data, int nthreads, long ntrials) {
    int count = 0;
    for (int i = 0; i < nthreads; ++i) {
        pthread_join(thread_handles[i], NULL);
        count += data[i].local_result;
    }
    return 4 * ((float) count / ntrials);
}

void print_result(double result, double time) {
    printf("Result: %f\n", result);
    printf("Time: %f s\n", time);
}