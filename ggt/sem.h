/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute
 * this software, either in source code form or as a compiled binary, for any
 * purpose, commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors of
 * this software dedicate any and all copyright interest in the software to the
 * public domain. We make this dedication for the benefit of the public at
 * large and to the detriment of our heirs and successors. We intend this
 * dedication to be an overt act of relinquishment in perpetuity of all present
 * and future rights to this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef GGGGT_SEM_H
#define GGGGT_SEM_H 1

#include "sem-types.h"

#if GGT_GREEN
static void ggt_sem_init(ggt_sem_t *sem, unsigned int val) {
    GGT_INIT(sem->waiting);
    sem->val = val;
#if GGT_SUPP_THREADS
    pthread_mutex_init(sem->lock, NULL);
#endif
}

static void ggt_sem_destroy(ggt_sem_t *sem) {
    GGT_FREE(sem->waiting);
}

static void ggt_sem_post(ggt_thread_t *thr, ggt_sem_t *sem) {
#if GGT_SUPP_THREADS
    pthread_mutex_lock(sem->lock);
#endif
    if (sem->waiting.next)
        GGT_WAKE_ONE(sem->waiting);
    else
        sem->val++;
#if GGT_SUPP_THREADS
    pthread_mutex_unlock(sem->lock);
#endif
}

#define GGT_SEM_WAIT(sem) do { \
    GGGGT_IF_THREADS({ \
        pthread_mutex_lock((sem)->lock); \
    }); \
    if ((sem)->val) { \
        (sem)->val--; \
        GGGGT_IF_THREADS({ \
            pthread_mutex_unlock((sem)->lock); \
        }); \
    } else { \
        GGGGT_SLEEP_NY((sem)->waiting); \
        GGGGT_IF_THREADS({ \
            pthread_mutex_unlock((sem)->lock); \
        }); \
        GGT_YIELD(); \
    } \
} while (0)

static int ggt_sem_trywait(ggt_sem_t *sem) {
#if GGT_SUPP_THREADS
    pthread_mutex_lock(sem->lock);
#endif
    if (sem->val) {
        sem->val--;
#if GGT_SUPP_THREADS
        pthread_mutex_unlock(sem->lock);
#endif
        return 0;
    } else {
#if GGT_SUPP_THREADS
        pthread_mutex_unlock(sem->lock);
#endif
        return -1;
    }
}

#else /* !GGT_GREEN */
#define ggt_sem_init(sem, val) ggt_native_sem_init((sem), (val))
#define ggt_sem_destroy(sem) ggt_native_sem_destroy((sem))
#define ggt_sem_post(thr, sem) ggt_native_sem_post((sem))
#define GGT_SEM_WAIT(sem) ggt_native_sem_wait((sem))
#define ggt_sem_trywait(sem) ggt_native_trywait((sem))

#endif

#endif
