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
 * POSIX native threads
 */

#ifndef GGT_NATIVE_THREADS_POSIX_H
#define GGT_NATIVE_THREADS_POSIX_H 1

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <sched.h>

typedef pthread_t ggt_native_thread_t;

static void ggt_native_thread_create(
    pthread_t *thr, void *(*proc)(void *), void *arg
) {
    int ret = pthread_create(thr, NULL, proc, arg);
    if (ret != 0) {
        perror("pthread_create");
        exit(1);
    }
}

#define ggt_native_thread_join(thr) pthread_join(*(thr), NULL)
#define ggt_native_thread_exit() pthread_exit(NULL)
#define ggt_native_thread_yield() sched_yield()

#endif
