#ifndef GGT_NATIVE_THREADS_POSIX_H
#define GGT_NATIVE_THREADS_POSIX_H 1

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <sched.h>

typedef pthread_t ggt_native_thread_t;

static void ggt_native_thread_create(
    pthread_t *thr, void *(*proc)(void *), void *arg
) {
    int ret = pthread_create(thr, NULL, proc, arg);
    if (ret != 0) {
        perror("pthread_create");
        exit(1);
    }
}

#define ggt_native_thread_join(thr) pthread_join(*(thr), NULL)
#define ggt_native_thread_exit() pthread_exit(NULL)
#define ggt_native_thread_yield() sched_yield()

#endif
