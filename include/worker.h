#ifndef WORKER_H
#define WORKER_H
#include <pthread.h>
struct PthreadData {
    int epfd;
    int efd;
    pthread_t tid;
};

extern struct PthreadData worker[4];
void* work(void* arg);

#endif
