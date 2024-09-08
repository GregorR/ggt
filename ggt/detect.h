#ifndef GGGGT_DETECT_H
#define GGGGT_DETECT_H 1

#if defined(unix) || defined(__unix) || defined(__unix__) || \
    (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
#include <pthread.h>
#include <dispatch/dispatch.h>

#define GGGGT_THREADS_NATIVE 1
#define GGGGT_THREADS_POSIX 1
#define GGGGT_THREADS_MACOSX 1

#elif defined(_POSIX_THREADS)
#include <pthread.h>

#define GGGGT_THREADS_NATIVE 1
#define GGGGT_THREADS_POSIX 1

#elif defined(_WIN32)
#define GGGGT_THREADS_NATIVE 1
#define GGGGT_THREADS_WINDOWS 1

#endif

#ifndef GGGGT_THREADS_NATIVE
#define GGGGT_THREADS_NATIVE 0
#endif

#ifndef GGGGT_THREADS_POSIX
#define GGGGT_THREADS_POSIX 0
#endif

#ifndef GGGGT_THREADS_MACOSX
#define GGGGT_THREADS_MACOSX 0
#endif

#ifndef GGGGT_THREADS_WINDOWS
#define GGGGT_THREADS_WINDOWS 0
#endif

#endif
