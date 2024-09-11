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
 * Green thread wake.
 */

#include "ggt/green.h"

#if GGT_COMP_THREADS == GGT_SUPP_THREADS

void GGGGT_THR(ggggtGreenWakeOne)(ggt_thread_list_t *list, ggt_thread_t *thr) {
#if GGT_SUPP_THREADS
    ggt_native_sem_wait(list->lock);
#endif
    if (list->next) {
        ggt_thread_t *othr = list->next;
        list->next = othr->next;
#if GGT_SUPP_THREADS
        ggt_native_sem_post(list->lock);
        ggt_native_sem_wait(thr->lock);
        othr->lock = thr->lock;
#endif
        if (thr->next)
            thr->next->prev = othr;
        othr->next = thr->next;
        thr->next = othr;
        othr->prev = thr;
#if GGT_SUPP_THREADS
        ggt_native_sem_post(thr->lock);
    } else {
        ggt_native_sem_post(list->lock);
#endif
    }
}

void GGGGT_THR(ggggtGreenWake)(ggt_thread_list_t *list, ggt_thread_t *thr) {
    while (list->next)
        GGT_WAKE_ONE(*list);
}

#endif
