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

/*
 * Gregor's Green Threads.
 * Wrapper to use native threads with GGT macros.
 */

#ifndef GGGGT_NATIVE_H
#define GGGGT_NATIVE_H 1

#include <stdio.h>
#include <stdlib.h>

#include "native/threads.h"
#include "native/sem.h"

#define GGT_GREEN 0
#define GGT_TEAL 0
#define GGT_NATIVE 1

#undef GGT_SUPP_THREADS
#define GGT_SUPP_THREADS 1

#undef GGT_SUPP_JOIN
#define GGT_SUPP_JOIN 1

#ifndef GGT_SUPP_EXCEPTIONS
#define GGT_SUPP_EXCEPTIONS 1
#endif

#ifndef GGT_SUPP_SJLJ
#define GGT_SUPP_SJLJ 1
#endif

#if GGT_SUPP_EXCEPTIONS || GGT_SUPP_SJLJ
#include <setjmp.h>
#endif

typedef struct ggt_thread_list_t {
    struct ggt_thread_t *next;
    ggt_native_sem_t lock[1];
} ggt_thread_list_t;

typedef struct ggt_thread_t {
    struct ggt_thread_t *next, *prev;
    ggt_native_thread_t pth;
    ggt_native_sem_t sleep;
    ggt_native_sem_t *lock;
#if GGT_SUPP_EXCEPTIONS
    void *throw_;
    jmp_buf *catch_;
#endif
} ggt_thread_t;

#if GGT_SUPP_SJLJ
typedef jmp_buf ggt_jmpbuf_t;
#endif

#if GGT_SUPP_EXCEPTIONS
#define GGGGT_EXC_INIT_THR(thr) do { \
    (thr)->throw_ = NULL; \
    (thr)->catch_ = NULL; \
} while (0)
#define GGGGT_EXC_LOCALS() \
    jmp_buf catch_, *lcatch_
#else
#define GGGGT_EXC_INIT_THR(thr)
#define GGGGT_EXC_LOCALS()
#endif

#define GGT(name, params, locals, trans) \
void *name params { \
    struct locals l[1]; \
    GGGGT_EXC_LOCALS(); \
    trans

#define GGT_E(name, params, locals, trans) \
struct name ## Locals locals; \
struct name ## Arg { \
    ggt_thread_t *thr; \
    ggt_native_sem_t threadReady; \
    ggt_native_sem_t *localsReady; \
    struct name ## Locals *l; \
}; \
static void *name ## Runner(void *); \
void name params { \
    struct name ## Arg arg; \
    struct name ## Locals *l; \
    arg.thr = thr; \
    ggt_native_sem_init(&arg.threadReady, 0); \
    ggt_native_sem_init(&thr->sleep, 0); \
    GGGGT_EXC_INIT_THR(thr); \
    ggt_native_thread_create(&thr->pth, name ## Runner, (void *) &arg); \
    ggt_native_sem_wait(&arg.threadReady); \
    ggt_native_sem_destroy(&arg.threadReady); \
    l = arg.l; \
    trans \
    ggt_native_sem_post(arg.localsReady); \
} \
static void *name ## Runner(void *argP) { \
    struct name ## Arg *arg = (struct name ## Arg *) argP; \
    ggt_thread_t *thr = arg->thr; \
    struct name ## Locals l[1]; \
    GGGGT_EXC_LOCALS(); \
    ggt_native_sem_t localsReady; \
    arg->l = l; \
    ggt_native_sem_init(&localsReady, 0); \
    arg->localsReady = &localsReady; \
    ggt_native_sem_post(&arg->threadReady); \
    ggt_native_sem_wait(&localsReady); \
    ggt_native_sem_destroy(&localsReady);

#if GGT_SUPP_EXCEPTIONS
#define GGT_RETURN() do { \
    if (thr->catch_ == &catch_) \
        longjmp(catch_, 1); \
    return NULL; \
} while (0)
#else
#define GGT_RETURN() return NULL
#endif

#define GGT_END() \
    GGT_RETURN(); \
}

#define GGT_YIELD() ggt_native_thread_yield()

#define GGT_YIELD_UNTIL(cond) \
while (!(cond)) \
    GGT_YIELD()

#if GGT_SUPP_EXCEPTIONS
#define GGT_CALL(name, args) do { \
    name args; \
    if (thr->throw_) \
        GGT_RETURN(); \
} while (0)
#else
#define GGT_CALL(name, args) name args
#endif

#define GGT_INIT(list) do { \
    (list).next = NULL; \
    ggt_native_sem_init((list).lock, 1); \
} while (0)

#define GGT_FREE(list) do { \
    ggt_native_sem_destroy((list).lock); \
} while (0)

#define GGT_SPAWN(list, thr, name, args) do { \
    ggt_native_sem_wait((list).lock); \
    (thr).lock = (list).lock; \
    if ((list).next) { \
        (list).next->prev = &(thr); \
        (thr).next = (list).next; \
    } else { \
        (thr).next = NULL; \
    } \
    (list).next = &(thr); \
    (thr).prev = (ggt_thread_t *) (void *) &(list); \
    ggt_native_sem_post((list).lock); \
    name args; \
} while (0)

#if GGT_SUPP_EXCEPTIONS
#define GGT_CATCH(ex, block) do { \
    lcatch_ = thr->catch_; \
    thr->catch_ = &catch_; \
    if (setjmp(catch_) != 0) { \
        void *ex = thr->throw_; \
        thr->catch_ = lcatch_; \
        thr->throw_ = NULL; \
        block \
        return NULL; \
    } \
} while (0)

#define GGT_THROW(ex) do { \
    thr->throw_ = (ex); \
    if (thr->throw_) { \
        if (thr->catch_) \
            longjmp(*thr->catch_, 1); \
        else \
            ggt_native_thread_exit(); \
    } \
    GGT_RETURN(); \
} while (0)
#endif

#if GGT_SUPP_SJLJ
#define GGT_SETJMP(jmpbuf, block) do { \
    if (setjmp((jmpbuf)) != 0) { \
        block \
    } \
} while (0)

#define GGT_LONGJMP(jmpbuf) longjmp((jmpbuf), 1)
#endif

void ggggtNativeSleep(ggt_thread_list_t *, ggt_thread_t *);
#define GGT_SLEEP(list, block) do { \
    ggggtNativeSleep(&(list), thr); \
    block \
    ggt_native_sem_wait(&thr->sleep); \
} while (0)
#define GGGGT_SLEEP(list, _, block) GGT_SLEEP(list, block)

void ggggtNativeWakeOne(ggt_thread_list_t *, ggt_thread_t *);
#define GGT_WAKE_ONE(list) ggggtNativeWakeOne(&(list), thr)

void ggggtNativeWake(ggt_thread_list_t *, ggt_thread_t *);
#define GGT_WAKE(list) ggggtNativeWake(&(list), thr)

static void ggtRun(ggt_thread_list_t *list) {
    ggt_thread_t *thr, *nthr;
    ggt_native_sem_wait(list->lock);
    thr = list->next;
    while (thr) {
        nthr = thr->next;
        /* FIXME: This is a race */
        ggt_native_sem_post(list->lock);
        ggt_native_thread_join(&thr->pth);
        ggt_native_sem_wait(list->lock);
        thr = nthr;
    }
    ggt_native_sem_post(list->lock);
}

#define GGT_JOIN(thr) \
    ggt_native_thread_join(&((thr).pth))

#endif
