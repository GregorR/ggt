#ifndef GGGGT_NATIVE_H
#define GGGGT_NATIVE_H 1

#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <stdlib.h>

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
    pthread_mutex_t lock[1];
} ggt_thread_list_t;

typedef struct ggt_thread_t {
    struct ggt_thread_t *next, *prev;
    pthread_t pth;
    sem_t sleep;
    pthread_mutex_t *lock;
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
    struct locals l_, *l; \
    GGGGT_EXC_LOCALS(); \
    l = &l_; \
    trans

#define GGT_E(name, params, locals, trans) \
struct name ## Locals locals; \
struct name ## Arg { \
    ggt_thread_t *thr; \
    sem_t threadReady; \
    sem_t *localsReady; \
    struct name ## Locals *l; \
}; \
void *name ## Runner(void *); \
void name params { \
    struct name ## Arg arg; \
    struct name ## Locals *l; \
    int pr; \
    arg.thr = thr; \
    sem_init(&arg.threadReady, 0, 0); \
    sem_init(&thr->sleep, 0, 0); \
    GGGGT_EXC_INIT_THR(thr); \
    pr = pthread_create(&thr->pth, NULL, name ## Runner, (void *) &arg); \
    if (pr != 0) { \
        perror("pthread_create"); \
        exit(1); \
    } \
    sem_wait(&arg.threadReady); \
    sem_destroy(&arg.threadReady); \
    l = arg.l; \
    trans \
    sem_post(arg.localsReady); \
} \
void *name ## Runner(void *argP) { \
    struct name ## Arg *arg = (struct name ## Arg *) argP; \
    ggt_thread_t *thr = arg->thr; \
    struct name ## Locals l_, *l; \
    GGGGT_EXC_LOCALS(); \
    sem_t localsReady; \
    arg->l = l = &l_; \
    sem_init(&localsReady, 0, 0); \
    arg->localsReady = &localsReady; \
    sem_post(&arg->threadReady); \
    sem_wait(&localsReady); \
    sem_destroy(&localsReady);

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

#define GGT_YIELD() sched_yield()

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
    pthread_mutex_init((list).lock, NULL); \
} while (0)

#define GGT_SPAWN(list, thr, name, args) do { \
    pthread_mutex_lock((list).lock); \
    (thr).lock = (list).lock; \
    if ((list).next) { \
        (list).next->prev = &(thr); \
        (thr).next = (list).next; \
    } else { \
        (thr).next = NULL; \
    } \
    (list).next = &(thr); \
    (thr).prev = (ggt_thread_t *) (void *) &(list); \
    pthread_mutex_unlock((list).lock); \
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
    if (thr->catch_) \
        longjmp(*thr->catch_, 1); \
    else \
        pthread_exit(NULL); \
    return NULL; \
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

#define GGT_SLEEP(list) do { \
    pthread_mutex_lock((thr).lock); \
    if (thr->prev) \
        thr->prev->next = thr->next; \
    if (thr->next) \
        thr->next->prev = thr->prev; \
    pthread_mutex_unlock((thr).lock); \
    pthread_mutex_lock((list).lock); \
    (thr).lock = (list).lock; \
    if ((list).next) { \
        (list).next->prev = thr; \
        thr->next = (list).next; \
    } else { \
        thr->next = NULL; \
    } \
    (list).next = thr; \
    thr->prev = (ggt_thread_t *) (void *) &(list); \
    pthread_mutex_unlock((list).lock); \
    sem_wait(&thr->sleep); \
} while (0)

#define GGT_WAKE_ONE(list) do { \
    pthread_mutex_lock((list).lock); \
    if ((list).next) { \
        ggt_thread_t *othr_ = (list).next; \
        (list).next = othr_->next; \
        pthread_mutex_unlock((list).lock); \
        pthread_mutex_lock((thr).lock); \
        othr_->lock = (thr).lock; \
        othr_->next = thr->next; \
        thr->next = othr_; \
        othr_->prev = thr; \
        pthread_mutex_unlock((thr).lock); \
        sem_post(&othr_->sleep); \
    } else { \
        pthread_mutex_unlock((list).lock); \
    } \
} while (0)

#define GGT_WAKE(list) do { \
    while ((list).next) { \
        GGT_WAKE_ONE(list); \
    } \
} while (0)

#define GGT_JOIN(thr) \
    pthread_join((thr).pth, NULL)

static void ggtRun(ggt_thread_list_t *list) {
    ggt_thread_t *thr;
    for (thr = list->next; thr; thr = thr->next)
        pthread_join(thr->pth, NULL);
}

#endif
