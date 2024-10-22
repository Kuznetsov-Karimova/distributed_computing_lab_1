#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "point.h"
#include <sys/time.h>

#define TIME(time)                           \
    {                                           \
        struct timeval t;                       \
        gettimeofday(&t, NULL);                 \
        time = 1.0e-6*t.tv_usec + t.tv_sec; \
    }
    
int isInCircle(struct Point* point) {
    if (distFromCenter(point) <= 1) {
        return 1;
    } else {
        return 0;
    }
};

struct Data{
    long local_trials;
    unsigned int seed;
    int local_result;
};

void* getPI(void* data) {
    struct Data* local_data = (struct Data*)data;
    struct Point point;
    for (int numPoint = 0; numPoint < local_data->local_trials; ++numPoint) {
        point.x = -1+2*((float)rand_r(&local_data->seed))/RAND_MAX;
        point.y = -1+2*((float)rand_r(&local_data->seed))/RAND_MAX;
        if (isInCircle(&point)) {
           ++(local_data->local_result);
        }
    }
    return (void*)(&local_data->local_result);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        exit(EXIT_FAILURE);
    }
    int nthreads = atoi(argv[1]);
    int ntrials = atol(argv[2]);

    pthread_t* thread_handles = malloc(nthreads * sizeof(pthread_t));
    struct Data* data = malloc(nthreads * sizeof(struct Data));
    int commSize = ntrials / nthreads;
    double start, end;
    TIME(start);
    for (int i = 0; i < nthreads; ++i) {
        data[i].seed = rand();
        data[i].local_trials = commSize;
        data[i].local_result = 0;
        pthread_create(&thread_handles[i], NULL, getPI, &data[i]);
    } 
    int count = 0;
    for (int i = 0; i < nthreads; ++i) {
        pthread_join(thread_handles[i], NULL);
        count += data[i].local_result;
    }
    printf("Result: %f\n", 4 * ((float) count / ntrials));
    TIME(end)
    printf("Time: %f\n", end - start);
    return 0;
}