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

#ifndef GGT_NATIVE_THREADS_WINDOWS_H
#define GGT_NATIVE_THREADS_WINDOWS_H 1

#include <stdlib.h>

#include <windows.h>

typedef HANDLE ggt_native_thread_t;

static void ggt_native_thread_create(
    HANDLE *thr, void *(*proc)(void *), void *arg
) {
    *thr = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) proc, arg, 0, NULL);
    if (!*thr)
        exit(1);
}

#define ggt_native_thread_join(thr) WaitForSingleObject(*(thr), INFINITE)
#define ggt_native_thread_exit() ExitThread(0)
#define ggt_native_thread_yield() SwitchToThread()

#endif
