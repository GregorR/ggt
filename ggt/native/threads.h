#ifndef GGGGT_NATIVE_THREADS_H
#define GGGGT_NATIVE_THREADS_H 1

#include "detect.h"

#if GGGGT_THREADS_POSIX
#include "threads-posix.h"

#elif GGGGT_THREADS_WINDOWS
#include "threads-windows.h"

#endif

#endif
