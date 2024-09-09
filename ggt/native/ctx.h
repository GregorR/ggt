#ifndef GGGGT_NATIVE_CTX_H
#define GGGGT_NATIVE_CTX_H 1

#include "detect-ctx.h"

#if GGGGT_SUPP_CTX_SWITCH == GGGGT_SUPP_CTX_SWITCH_X86_64
static void ggtContextCreate(
    ggt_thread_t *thr, void *sp,
    void (*routine)(ggt_thread_t *, void *), void *arg
) {
    if (setjmp(thr->prev->ctx) == 0) {
        __asm__ volatile(
            "movq %0, %%rdi\n"
            "movq %1, %%rsi\n"
            "movq %2, %%rsp\n"
            "jmpq *%3"
            :
            : "r" (thr),
              "r" (arg),
              "r" (sp),
              "r" (routine)
            : "rdi", "rsi"
        );
    }
}
#endif

#endif
