#ifndef GGGGT_SEM_TYPES_H
#define GGGGT_SEM_TYPES_H 1

#if GGT_SUPP_THREADS
#include "detect.h"
#endif

#if GGT_GREEN
typedef struct ggt_sem_t {
    ggt_thread_list_t waiting;
    unsigned int val;
#if GGT_SUPP_THREADS
    pthread_mutex_t lock[1];
#endif
} ggt_sem_t;

#else /* !GGT_GREEN */
#include "native-sem.h"
typedef ggt_native_sem_t ggt_sem_t;

#endif

#endif
