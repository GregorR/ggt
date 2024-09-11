#ifndef GGGGT_NATIVE_CTX_H
#define GGGGT_NATIVE_CTX_H 1

#include "detect-ctx.h"

static void ggtContextCreate(
    ggt_thread_t *thr, void *sp,
    void (*routine)(ggt_thread_t *, void *), void *arg
) {
    if (setjmp(thr->prev->ctx) == 0) {
#if GGGGT_SUPP_CTX_SWITCH == GGGGT_SUPP_CTX_SWITCH_X86
        __asm__ volatile(
            "mov %2, %%esp\n"
            "push %1\n"
            "push %0\n"
            "push $0\n"
            "jmp *%3"
            :
            : "r" (thr),
              "r" (arg),
              "r" (sp),
              "r" (routine)
        );

#elif GGGGT_SUPP_CTX_SWITCH == GGGGT_SUPP_CTX_SWITCH_X86_64
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

#elif GGGGT_SUPP_CTX_SWITCH == GGGGT_SUPP_CTX_SWITCH_ARM
        __asm__ volatile(
            "mov r0, %0\n"
            "mov r1, %1\n"
            "mov sp, %2\n"
            "bx %3\n"
            :
            : "r" (thr),
              "r" (arg),
              "r" (sp),
              "r" (routine)
            : "r0", "r1"
        );

#elif GGGGT_SUPP_CTX_SWITCH == GGGGT_SUPP_CTX_SWITCH_ARM64
        __asm__ volatile(
            "mov x0, %0\n"
            "mov x1, %1\n"
            "mov sp, %2\n"
            "br %3\n"
            :
            : "r" (thr),
              "r" (arg),
              "r" (sp),
              "r" (routine)
            : "x0", "x1"
        );

#endif
    }
}

#endif
