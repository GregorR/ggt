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
 * Detection of native threading capabilities.
 */

#ifndef GGGGT_NATIVE_DETECT_H
#define GGGGT_NATIVE_DETECT_H 1

#if defined(unix) || defined(__unix) || defined(__unix__) || \
    (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define GGGGT_THREADS_NATIVE 1
#define GGGGT_THREADS_POSIX 1
#define GGGGT_THREADS_MACOSX 1

#elif defined(_POSIX_THREADS)
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
