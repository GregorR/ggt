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
 * POSIX native semaphores
 */

#ifndef GGGGT_NATIVE_SEM_POSIX_H
#define GGGGT_NATIVE_SEM_POSIX_H 1

#include <semaphore.h>

typedef sem_t ggt_native_sem_t;

#define ggt_native_sem_init(sem, value) sem_init((sem), 0, (value))
#define ggt_native_sem_destroy(sem) sem_destroy((sem))
#define ggt_native_sem_post(sem) sem_post((sem))
#define ggt_native_sem_wait(sem) sem_wait((sem))
#define ggt_native_sem_trywait(sem) sem_trywait((sem))

#endif
