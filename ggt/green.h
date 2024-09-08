#ifndef GGGGT_GREEN_H
#define GGGGT_GREEN_H 1

#include <stdlib.h>

#ifndef GGT_SUPP_EXCEPTIONS
#define GGT_SUPP_EXCEPTIONS 1
#endif

#ifndef GGT_SUPP_SJLJ
#define GGT_SUPP_SJLJ 1
#endif

#define GGGGT_STATE_INIT (0)
#define GGGGT_STATE_DONE (-1)

typedef struct ggt_thread_list_t {
    struct ggt_thread_t *next;
} ggt_thread_list_t;

typedef struct ggt_thread_t {
    struct ggt_thread_t *next, *prev;
    struct ggt_stack_t *stack;
    ggt_thread_list_t joined;
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

#if GGT_SUPP_EXCEPTIONS || GGT_SUPP_SJLJ
#define GGGGT_EXC_INIT_THR(thr) do { \
    (thr).throw_ = NULL; \
} while (0)
#define GGGGT_EXC_INIT_STACK(stack) do { \
    (stack)->catch_ = 0; \
} while (0)
#define GGGGT_EXC_CALL_DONE(stack) \
    ((stack)->state != GGGGT_STATE_DONE && !(stack)->catch_)
#else
#define GGGGT_EXC_INIT_THR(thr)
#define GGGGT_EXC_INIT_STACK(stack)
#define GGGGT_EXC_CALL_DONE(stack) \
    ((stack)->state != GGGGT_STATE_DONE)
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
    GGGGT_EXC_INIT_STACK(stack); \
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
        case GGGGT_STATE_INIT:

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
    stack->state = __LINE__; return; case __LINE__: \
} while (0)

#define GGT_SAVE(to) do { \
    (to) = __LINE__; case __LINE__: \
} while (0)

#define GGT_RESUME(from) do { \
    stack->state = (from); \
    return; \
} while (0)

#define GGT_YIELD_UNTIL(cond) \
while (!(cond)) \
    GGT_YIELD()

#define GGT_CALL(name, args) do { \
    stack->state = __LINE__; stackNext = name args; if (GGGGT_EXC_CALL_DONE(stackNext)) return; thr->stack = stack; free(stackNext); case __LINE__: \
} while (0)

#define GGT_INIT(list) do { \
    (list).next = NULL; \
} while (0)

#define GGT_SPAWN(list, thr, name, args) do { \
    (thr).stack = NULL; \
    (thr).joined.next = NULL; \
    GGGGT_EXC_INIT_THR(thr); \
    if ((list).next) { \
        (list).next->prev = &(thr); \
        (thr).next = (list).next; \
    } else { \
        (thr).next = NULL; \
    } \
    (list).next = &(thr); \
    (thr).prev = (ggt_thread_t *) (void *) &(list); \
    name args; \
} while (0)

#define GGT_SLEEP(list) do { \
    if (thr->prev) \
        thr->prev->next = thr->next; \
    if (thr->next) \
        thr->next->prev = thr->prev; \
    if ((list).next) { \
        (list).next->prev = thr; \
        thr->next = (list).next; \
    } else { \
        thr->next = NULL; \
    } \
    (list).next = thr; \
    thr->prev = (ggt_thread_t *) (void *) &(list); \
    GGT_YIELD(); \
} while (0)

#define GGT_WAKE_ONE(list) do { \
    if ((list).next) { \
        ggt_thread_t *othr_ = (list).next; \
        (list).next = othr_->next; \
        if (thr->next) \
            thr->next->prev = othr_; \
        othr_->next = thr->next; \
        thr->next = othr_; \
        othr_->prev = thr; \
    } \
} while (0)

#define GGT_WAKE(list) do { \
    while ((list).next) { \
        GGT_WAKE_ONE(list); \
    } \
} while (0)

#define GGT_JOIN(thr) do { \
    if ((thr).stack) \
        GGT_SLEEP((thr).joined); \
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

static void ggtRun(ggt_thread_list_t *list) {
    ggt_thread_t *thr, *nthr;
    while (list->next) {
        for (thr = list->next; thr; thr = nthr) {
            nthr = thr->next;
            ggt_stack_t *stack = thr->stack;
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
                GGT_WAKE(thr->joined);
                thr->prev->next = thr->next;
                if (thr->next)
                    thr->next->prev = thr->prev;
            }
        }
    }
}

#endif
