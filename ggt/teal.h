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
 * Gregor's Green Threads
 * “Teal” threads: Threads implemented by setjmp, longjmp, and a
 * platform-specific stack jump primitive.
 */

#ifndef GGGGT_NATIVE_H
#define GGGGT_NATIVE_H 1

#include <stdio.h>
#include <stdlib.h>

#include <setjmp.h>

#define GGT_GREEN 0
#define GGT_TEAL 1
#define GGT_NATIVE 0

#ifndef GGT_SUPP_THREADS
#define GGT_SUPP_THREADS 0
#endif

#ifndef GGT_SUPP_JOIN
#define GGT_SUPP_JOIN 1
#endif

#ifndef GGT_SUPP_EXCEPTIONS
#define GGT_SUPP_EXCEPTIONS 1
#endif

#ifndef GGT_SUPP_SJLJ
#define GGT_SUPP_SJLJ 1
#endif

#if GGT_SUPP_THREADS
#include "native/detect.h"
#if !GGGGT_THREADS_NATIVE
#undef GGT_SUPP_THREADS
#define GGT_SUPP_THREADS 0
#else
#include "native/sem.h"
#endif
#endif

#if GGT_SUPP_THREADS
#define GGGGT_IF_THREADS(block) do block while(0)
#else
#define GGGGT_IF_THREADS(block)
#endif

#if GGT_SUPP_JOIN
#define GGGGT_IF_JOIN(block) do block while (0)
#else
#define GGGGT_IF_JOIN(block)
#endif

#if GGT_SUPP_EXCEPTIONS
#define GGGGT_IF_EXCEPTIONS(block) do block while(0)
#else
#define GGGGT_IF_EXCEPTIONS(block)
#endif

typedef struct ggt_thread_list_t {
    struct ggt_thread_t *prev, *next;
    jmp_buf ctx;
#if GGT_SUPP_THREADS
    ggt_native_sem_t lock[1];
#endif
} ggt_thread_list_t;

#if GGT_SUPP_JOIN
#include "sem-types.h"
#endif

typedef struct ggt_thread_t {
    struct ggt_thread_t *prev, *next;
    jmp_buf ctx;
    unsigned char *stack;
#if GGT_SUPP_THREADS
    ggt_native_sem_t *lock;
#endif
#if GGT_SUPP_JOIN
    ggt_sem_t joinLock;
    ggt_thread_list_t joined;
#endif
#if GGT_SUPP_EXCEPTIONS
    void *throw_;
    jmp_buf *catch_;
#endif
} ggt_thread_t;

#include "native/ctx.h"

#if GGT_SUPP_SJLJ
typedef jmp_buf ggt_jmpbuf_t;
#endif

#define GGGGT_STACK_SZ 65536

#if GGT_SUPP_EXCEPTIONS
#define GGGGT_EXC_LOCALS() \
    jmp_buf catch_, *lcatch_;
#else
#define GGGGT_EXC_LOCALS()
#endif
              
#define GGT(name, params, locals, trans) \
void name params { \
    struct locals l[1]; \
    GGGGT_EXC_LOCALS(); \
    trans

static void ggggtExit(ggt_thread_t *thr);

#define GGT_E(name, params, locals, trans) \
struct name ## Locals locals; \
static void name ## Runner(ggt_thread_t *, void *); \
static void name ## Real(ggt_thread_t *, struct name ## Locals *); \
void name params { \
    struct name ## Locals *l; \
    thr->stack = malloc(GGGGT_STACK_SZ); \
    if (thr->stack == NULL) { \
        perror("malloc"); \
        exit(1); \
    } \
    l = (struct name ## Locals *) \
        (thr->stack + GGGGT_STACK_SZ - sizeof(struct name ## Locals)); \
    GGGGT_IF_EXCEPTIONS({ \
        thr->throw_ = NULL; \
        thr->catch_ = NULL; \
    }); \
    trans \
    ggtContextCreate(thr, (unsigned char *) l, name ## Runner, (void *) l); \
} \
static void name ## Runner(ggt_thread_t *thr, void *l_) { \
    name ## Real(thr, (struct name ## Locals *) l_); \
    ggggtExit(thr); \
} \
static void name ## Real(ggt_thread_t *thr, struct name ## Locals *l) { \
    GGGGT_EXC_LOCALS();

#if GGT_SUPP_EXCEPTIONS
#define GGT_RETURN() do { \
    if (thr->catch_ == &catch_) \
        longjmp(catch_, 1); \
    return; \
} while (0)
#else
#define GGT_RETURN() return
#endif

#define GGT_END() \
    GGT_RETURN(); \
}

#define GGT_YIELD() do { \
    if (setjmp(thr->ctx) == 0) \
        longjmp(thr->next->ctx, 1); \
} while (0)

#define GGT_YIELD_UNTIL(cond) \
while (!(cond)) \
    GGT_YIELD()

#define GGT_CALL(name, args) name args

#define GGT_INIT(list) do { \
    (list).next = NULL; \
    GGGGT_IF_THREADS({ \
        ggt_native_sem_init((list).lock, 1); \
    }); \
} while (0)

#define GGT_FREE(list)

#define GGT_SPAWN(list, thr, name, args) do { \
    GGGGT_IF_THREADS({ \
        ggt_native_sem_wait((list).lock); \
        (thr).lock = (list).lock; \
    }); \
    GGGGT_IF_JOIN({ \
        ggt_sem_init(&(thr).joinLock, 1); \
        GGT_INIT((thr).joined); \
    }); \
    if ((list).next) { \
        (list).next->prev = &(thr); \
        (thr).next = (list).next; \
    } else { \
        (list).prev = &(thr); \
        (thr).next = (ggt_thread_t *) (void *) &(list); \
    } \
    (list).next = &(thr); \
    (thr).prev = (ggt_thread_t *) (void *) &(list); \
    GGGGT_IF_THREADS({ \
        ggt_native_sem_post((list).lock); \
    }); \
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
        return; \
    } \
} while (0)

#define GGT_THROW(ex) do { \
    thr->throw_ = (ex); \
    if (thr->throw_) { \
        if (thr->catch_) \
            longjmp(*thr->catch_, 1); \
        else \
            ggggtExit(thr); \
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

#define GGGGT_SLEEP_B(list, _, block) do { \
    ggt_thread_t *nthr; \
    GGGGT_IF_THREADS({ \
        ggt_native_sem_wait(thr->lock); \
    }); \
    nthr = thr->next; \
    thr->next->prev = thr->prev; \
    thr->prev->next = thr->next; \
    GGGGT_IF_THREADS({ \
        ggt_native_sem_post(thr->lock); \
        ggt_native_sem_wait((list).lock); \
        thr->lock = (list).lock; \
    }); \
    if ((list).next) { \
        (list).next->prev = thr; \
        thr->next = (list).next; \
    } else { \
        thr->next = NULL; \
    } \
    (list).next = thr; \
    thr->prev = (ggt_thread_t *) (void *) &(list); \
    if (setjmp(thr->ctx) == 0) { \
        GGGGT_IF_THREADS({ \
            ggt_native_sem_post((list).lock); \
        }); \
        block \
        longjmp(nthr->ctx, 1); \
    } \
} while (0)

#define GGT_SLEEP(list) do { \
    GGGGT_SLEEP_B(list, 0, {}); \
} while (0)

#define GGT_WAKE_ONE(list) do { \
    GGGGT_IF_THREADS({ \
        ggt_native_sem_wait((list).lock); \
    }); \
    if ((list).next) { \
        ggt_thread_t *othr_ = (list).next; \
        (list).next = othr_->next; \
        GGGGT_IF_THREADS({ \
            ggt_native_sem_post((list).lock); \
            ggt_native_sem_wait(thr->lock); \
            othr_->lock = thr->lock; \
        }); \
        othr_->next = thr->next; \
        thr->next = othr_; \
        othr_->prev = thr; \
        GGGGT_IF_THREADS({ \
            ggt_native_sem_post(thr->lock); \
        }); \
    } else { \
        GGGGT_IF_THREADS({ \
            ggt_native_sem_post((list).lock); \
        }); \
    } \
} while (0)

#define GGT_WAKE(list) do { \
    while ((list).next) { \
        GGT_WAKE_ONE(list); \
    } \
} while (0)

#if GGT_SUPP_JOIN
#include "sem.h"
#endif

static void ggtRun(ggt_thread_list_t *list) {
    while (list->next != (ggt_thread_t *) (void *) list) {
        if (setjmp(list->ctx) == 0)
            longjmp(list->next->ctx, 1);
    }
}

static void ggggtExit(ggt_thread_t *thr) {
#if GGT_SUPP_JOIN
    GGT_SEM_WAIT(&thr->joinLock);
    GGT_WAKE(thr->joined);
    ggt_sem_post(thr, &thr->joinLock);
#endif
#if GGT_SUPP_THREADS
    ggt_native_sem_wait(thr->lock);
#endif
    thr->next->prev = thr->prev;
    thr->prev->next = thr->next;
#if GGT_SUPP_THREADS
    ggt_native_sem_post(thr->lock);
#endif
#if GGT_SUPP_JOIN
    ggt_sem_destroy(&thr->joinLock);
#endif
    /* FIXME: Zapping our own stack! */
    free(thr->stack);
    thr->stack = NULL;
    longjmp(thr->next->ctx, 1);
}

#if GGT_SUPP_JOIN
#define GGT_JOIN(othr) do { \
    GGT_SEM_WAIT(&(othr).joinLock); \
    if ((othr).stack) { \
        GGGGT_SLEEP_B((othr).joined, 0, { \
            ggt_sem_post(thr, &(othr).joinLock); \
        }); \
    } else { \
        ggt_sem_post(thr, &(othr).joinLock); \
    } \
} while (0)
#endif

#endif
