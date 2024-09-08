#ifndef GGGGT_NATIVE_SEM_H
#define GGGGT_NATIVE_SEM_H 1

#include "detect.h"

#if GGGGT_THREADS_MACOSX
#include "native-sem/macosx.h"

#elif GGGGT_THREADS_POSIX
#include "native-sem/posix.h"

#elif GGGGT_THREADS_WINDOWS
#include "native-sem/windows.h"

#endif

#endif
