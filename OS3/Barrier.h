//
// Created by owner on 5/26/2020.
//

#ifndef OS3_BARRIER_H
#define OS3_BARRIER_H

#include <pthread.h>

// a multiple use barrier

class Barrier {
public:
    Barrier(int numThreads);
    ~Barrier();
    void barrier();

private:
    pthread_mutex_t mutex;
    pthread_cond_t cv;
    int count;
    int numThreads;
};

#endif //OS3_BARRIER_H
