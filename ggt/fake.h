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
 * Gregor's Green Threads.
 * Fake implementation.
 */

#ifndef GGGGT_FAKE_H
#define GGGGT_FAKE_H 1

#include <stdio.h>
#include <stdlib.h>

#define GGT_FAKE 1
#define GGT_GREEN 0
#define GGT_TEAL 0
#define GGT_NATIVE 0

#undef GGT_SUPP_THREADS
#define GGT_SUPP_THREADS 0

#ifndef GGT_SUPP_JOIN
#define GGT_SUPP_JOIN 1
#endif

#ifndef GGT_SUPP_EXCEPTIONS
#define GGT_SUPP_EXCEPTIONS 1
#endif

#ifndef GGT_SUPP_SJLJ
#define GGT_SUPP_SJLJ 1
#endif

typedef unsigned char ggt_thread_list_t;

typedef unsigned char ggt_thread_t;

#if GGT_SUPP_SJLJ
#include <setjmp.h>
typedef jmp_buf ggt_jmpbuf_t;
#endif

typedef void *ggt_ret_t;

#define GGT(name, params, locals, trans) \
ggt_ret_t name params { \
    locals \
    trans

#define GGT_E GGT

#define GGT_P(t, x)
#define GGT_L(x) (x)
#define GGT_T(x) (void) 0
#define GGT_EP GGT_P
#define GGT_EL GGT_L
#define GGT_ET GGT_T

#define GGT_RETURN() return NULL

#define GGT_END() \
    GGT_RETURN(); \
}

#define GGT_YIELD() (void) 0

#define GGT_YIELD_UNTIL(cond) \
while (!(cond)) \
    GGT_YIELD()


#define GGT_CALL(name, args) name args

#define GGT_INIT(list) (void) 0

#define GGT_FREE(list) (void) 0

#define GGT_SPAWN(list, thr, name, args) name args

#define GGGGT_SLEEP(list, yieldOffset, block) abort()
#define GGT_SLEEP(list, block) GGGGT_SLEEP(list, 1, block)

#define GGT_WAKE_ONE(list) (void) 0
#define GGT_WAKE(list) (void) 0

#if GGT_SUPP_EXCEPTIONS
#define GGT_CATCH(ex, block) (void) 0

#define GGT_THROW(ex) abort()
#endif

#if GGT_SUPP_SJLJ
#define GGT_SETJMP(jmpbuf, block) do { \
    if (setjmp(&(jmpbuf)) != 0) { \
        block \
    } \
} while (0)

#define GGT_LONGJMP(jmpbuf) longjmp(&(jmpbuf), 1)
#endif


#define GGT_RUN(list) (void) 0

#if GGT_SUPP_JOIN
#define GGT_JOIN(othr) (void) 0
#endif

#endif
