#ifndef GGGGT_NATIVE_SEM_MACOSX_H
#define GGGGT_NATIVE_SEM_MACOSX_H 1

#include <pthread.h>
#include <dispatch/dispatch.h>

typedef dispatch_semaphore_t ggt_native_sem_t;

#define ggt_native_sem_destroy(sem) dispatch_release(*(sem))
#define ggt_native_sem_post(sem) dispatch_semaphore_signal(*(sem))
#define ggt_native_sem_wait(sem) dispatch_semaphore_wait(*(sem), DISPATCH_TIME_FOREVER)
#define ggt_native_sem_trywait(sem) dispatch_semaphore_wait(*(sem), DISPATCH_TIME_NOW)

static void ggc_native_sem_init(
    dispatch_semaphore_t *sem, unsigned int val
) {
    *sem = dispatch_semaphore_create(val);
    while (val--)
        ggt_native_sem_post(sem);
}

#endif
