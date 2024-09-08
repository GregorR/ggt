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
