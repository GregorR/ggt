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

#ifndef GGGGT_GREEN_H
#define GGGGT_GREEN_H 1

#include <stdio.h>
#include <stdlib.h>

#define GGT_GREEN 1

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
#define GGGGT_IF_EXCSJLJ(block) do block while (0)
#else
#define GGGGT_IF_EXCEPTIONS(block)
#endif

#if GGT_SUPP_SJLJ
#define GGGGT_IF_SJLJ(block) do block while(0)
#ifndef GGGGT_IF_EXCSJLJ
#define GGGGT_IF_SJLJ(block) do block while(0)
#endif
#else
#define GGGGT_IF_SJLJ(block)
#endif

#ifndef GGGGT_IF_EXCSJLJ
#define GGGGT_IF_EXCSJLJ(block)
#endif

#define GGGGT_STATE_INIT (0)
#define GGGGT_STATE_DONE (-1)

typedef struct ggt_thread_list_t {
    struct ggt_thread_t *next;
#if GGT_SUPP_THREADS
    ggt_native_sem_t lock[1];
#endif
} ggt_thread_list_t;

#if GGT_SUPP_JOIN
#include "sem-types.h"
#endif

typedef struct ggt_thread_t {
    struct ggt_thread_t *next, *prev;
    struct ggt_stack_t *stack;
#if GGT_SUPP_THREADS
    ggt_native_sem_t *lock;
#endif
#if GGT_SUPP_JOIN
    ggt_sem_t joinLock;
    ggt_thread_list_t joined;
#endif
#if GGT_SUPP_EXCEPTIONS || GGT_SUPP_SJLJ
    void *throw_;
#endif
} ggt_thread_t;

typedef struct ggt_stack_t {
    struct ggt_stack_t *next;
    void (*target)(ggt_thread_t *);
    int state;
#if GGT_SUPP_EXCEPTIONS || GGT_SUPP_SJLJ
    int catch_;
#endif
} ggt_stack_t;

#if GGT_SUPP_SJLJ
typedef struct ggt_jmpbuf_t {
    ggt_stack_t *stack;
    int state;
} ggt_jmpbuf_t;
#endif

#define GGT(name, params, locals, trans) \
struct name ## Locals locals; \
void name ## Runner(ggt_thread_t *); \
ggt_stack_t *name params { \
    ggt_stack_t *stack; \
    struct name ## Locals *l; \
    stack = malloc(sizeof(ggt_stack_t) + sizeof(struct name ## Locals)); \
    if (stack == NULL) { \
        perror("malloc"); \
        exit(1); \
    } \
    stack->next = thr->stack; \
    stack->target = name ## Runner; \
    stack->state = GGGGT_STATE_INIT; \
    GGGGT_IF_EXCSJLJ({ \
        (stack)->catch_ = 0; \
    }); \
    l = (struct name ## Locals *) (void *) (stack + 1); \
    trans \
    thr->stack = stack; \
    name ## Runner(thr); \
    return stack; \
} \
void name ## Runner(ggt_thread_t *thr) { \
    ggt_stack_t *stack = thr->stack, *stackNext; \
    struct name ## Locals *l = \
        (struct name ## Locals *) (void *) (stack + 1); \
    switch (stack->state) { \
        case GGGGT_STATE_INIT: (void) 0;

#define GGT_E(name, params, locals, trans) \
    GGT(name, params, locals, trans)

#define GGT_RETURN() do { \
    stack->state = GGGGT_STATE_DONE; \
    return; \
} while (0)

#define GGT_END() \
    GGT_RETURN(); \
    } \
}

#define GGT_YIELD() do { \
    stack->state = __LINE__; return; case __LINE__: (void) 0; \
} while (0)

#define GGT_SAVE(to) do { \
    (to) = __LINE__; case __LINE__: (void) 0; \
} while (0)

#define GGT_RESUME(from) do { \
    stack->state = (from); \
    return; \
} while (0)

#define GGT_YIELD_UNTIL(cond) \
while (!(cond)) \
    GGT_YIELD()


#if GGT_SUPP_EXCEPTIONS || GGT_SUPP_SJLJ
#define GGGGT_EXC_CALL_DONE(stack) \
    ((stack)->state != GGGGT_STATE_DONE && !(stack)->catch_)
#else
#define GGGGT_EXC_CALL_DONE(stack) \
    ((stack)->state != GGGGT_STATE_DONE)
#endif

#define GGT_CALL(name, args) do { \
    stack->state = __LINE__; stackNext = name args; if (GGGGT_EXC_CALL_DONE(stackNext)) return; thr->stack = stack; free(stackNext); case __LINE__: (void) 0; \
} while (0)

#define GGT_INIT(list) do { \
    (list).next = NULL; \
    GGGGT_IF_THREADS({ \
        ggt_native_sem_init((list).lock, 1); \
    }); \
} while (0)

#define GGT_FREE(list) do { \
    GGGGT_IF_THREADS({ \
        ggt_native_sem_destroy((list).lock); \
    }); \
} while (0)

#define GGT_SPAWN(list, thr, name, args) do { \
    (thr).stack = NULL; \
    GGT_INIT((thr).joined); \
    GGGGT_IF_JOIN({ \
        ggt_sem_init(&(thr).joinLock, 1); \
        GGT_INIT((thr).joined); \
    }); \
    GGGGT_IF_EXCSJLJ({ \
        (thr).throw_ = NULL; \
    }); \
    GGGGT_IF_THREADS({ \
        ggt_native_sem_wait((list).lock); \
        (thr).lock = (list).lock; \
    }); \
    if ((list).next) { \
        (list).next->prev = &(thr); \
        (thr).next = (list).next; \
    } else { \
        (thr).next = NULL; \
    } \
    (list).next = &(thr); \
    (thr).prev = (ggt_thread_t *) (void *) &(list); \
    GGGGT_IF_THREADS({ \
        ggt_native_sem_post((list).lock); \
    }); \
    name args; \
} while (0)

#define GGGGT_SLEEP_NY(list) do { \
    GGGGT_IF_THREADS({ \
        ggt_native_sem_wait(thr->lock); \
    }); \
    if (thr->prev) \
        thr->prev->next = thr->next; \
    if (thr->next) \
        thr->next->prev = thr->prev; \
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
    GGGGT_IF_THREADS({ \
        ggt_native_sem_post((list).lock); \
    }); \
} while (0)

#define GGT_SLEEP(list) do { \
    GGGGT_SLEEP_NY(list); \
    GGT_YIELD(); \
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
        if (thr->next) \
            thr->next->prev = othr_; \
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

#if GGT_SUPP_EXCEPTIONS
#define GGT_CATCH(ex, block) do { \
    stack->catch_ = __LINE__; if (0) { void *ex; int catchResume; case __LINE__: \
        ex = thr->throw_; \
        thr->throw_ = NULL; \
        catchResume = stack->catch_; \
        stack->catch_ = 0; \
        block \
        GGT_RETURN(); \
    } \
} while (0)

#define GGT_THROW(ex) do { \
    thr->throw_ = (ex); \
    GGT_RETURN(); \
} while (0)
#endif

#if GGT_SUPP_SJLJ
#define GGT_SETJMP(jmpbuf, block) do { \
    (jmpbuf).stack = stack; \
    (jmpbuf).state = __LINE__; if (0) { case __LINE__: \
        thr->throw_ = NULL; \
        block \
    } \
} while (0)

#define GGT_LONGJMP(jmpbuf) do { \
    (jmpbuf).stack->state = (jmpbuf).stack->catch_; \
    (jmpbuf).stack->catch_ = (jmpbuf).state; \
    thr->throw_ = (void *) (jmpbuf).stack; \
    GGT_RETURN(); \
} while (0)
#endif

#if GGT_SUPP_JOIN
#include "sem.h"
#endif

static void ggtRun(ggt_thread_list_t *list) {
    ggt_thread_t *thr, *nthr;
    ggt_stack_t *stack;
    while (list->next) {
        for (thr = list->next; thr; thr = nthr) {
#if GGT_SUPP_THREADS
            ggt_native_sem_wait(thr->lock);
#endif
            nthr = thr->next;
#if GGT_SUPP_THREADS
            ggt_native_sem_post(thr->lock);
#endif

            stack = thr->stack;
            if (stack) {
#if GGT_SUPP_EXCEPTIONS || GGT_SUPP_SJLJ
                if (thr->throw_) {
                    if (stack->catch_) {
                        int catchResume = stack->state;
                        stack->state = stack->catch_;
                        stack->catch_ = catchResume;
                    } else
                        stack->state = GGGGT_STATE_DONE;
                }
#endif
                if (stack->state == GGGGT_STATE_DONE) {
                    thr->stack = stack->next;
                    free(stack);
                } else {
                    stack->target(thr);
                }
            } else {
#if GGT_SUPP_JOIN
                if (ggt_sem_trywait(&thr->joinLock)) {
                    GGT_WAKE(thr->joined);
                    ggt_sem_post(thr, &thr->joinLock);
#endif

#if GGT_SUPP_THREADS
                    ggt_native_sem_wait(thr->lock);
#endif
                    thr->prev->next = thr->next;
                    if (thr->next)
                        thr->next->prev = thr->prev;
#if GGT_SUPP_THREADS
                    ggt_native_sem_post(thr->lock);
#endif

#if GGT_SUPP_JOIN
                    ggt_sem_destroy(&thr->joinLock);
                }
#endif
            }
        }
    }
}

#if GGT_SUPP_JOIN
#define GGT_JOIN(othr) do { \
    GGT_SEM_WAIT(&(othr).joinLock); \
    if ((othr).stack) { \
        GGGGT_SLEEP_NY((othr).joined); \
        ggt_sem_post(thr, &(othr).joinLock); \
        stack->state = __LINE__-1; return; case __LINE__-1: (void) 0; \
    } else { \
        ggt_sem_post(thr, &(othr).joinLock); \
    } \
} while (0)
#endif

#endif
