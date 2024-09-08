#ifndef GGGGT_NATIVE_SEM_H
#define GGGGT_NATIVE_SEM_H 1

#include "detect.h"

#if GGGGT_THREADS_MACOSX
#include "sem-macosx.h"

#elif GGGGT_THREADS_POSIX
#include "sem-posix.h"

#elif GGGGT_THREADS_WINDOWS
#include "sem-windows.h"

#endif

#endif
