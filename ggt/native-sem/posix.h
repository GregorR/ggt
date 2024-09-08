#ifndef GGGGT_NATIVE_SEM_POSIX_H
#define GGGGT_NATIVE_SEM_POSIX_H 1

#include <semaphore.h>

typedef sem_t ggt_native_sem_t;

#define ggt_native_sem_init(sem, value) sem_init((sem), 0, (value))
#define ggt_native_sem_destroy(sem) sem_destroy((sem))
#define ggt_native_sem_post(sem) sem_post((sem))
#define ggt_native_sem_wait(sem) sem_wait((sem))
#define ggt_native_sem_trywait(sem) sem_trywait((sem))

#endif
