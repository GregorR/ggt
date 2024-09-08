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

#include <stdio.h>

#ifdef GGT_TEAL
#include "ggt/teal.h"
#else
#include "ggt/green.h"
#endif

ggt_jmpbuf_t jb;

GGT(printer, (ggt_thread_t *thr, const char *toPrint), {
    const char *toPrint;
}, {
    l->toPrint = toPrint;
}) {
    printf("%s\n", l->toPrint);
    GGT_END();
}

GGT_E(a, (ggt_thread_t *thr), {
    int val;
}, {
}) {
    GGT_CATCH(ex, {
        printf("Catch!\n");
    });

    l->val = 0;
    while (l->val < 4096) {
        printf("Thread a (%d)\n", l->val++);
        l->val++;
        GGT_CALL(printer, (thr, "Hello, world!"));
        GGT_YIELD();
    }

    GGT_THROW((void *) 1);
    GGT_END();
}

GGT_E(b, (ggt_thread_t *thr), {
    int val;
    ggt_thread_t othr;
}, {
}) {
    GGT_SETJMP(jb, {
        printf("I sure do enjoy setjmp\n");
        GGT_RETURN();
    });

    l->val = 1024;
    GGT_SPAWN(*thr, l->othr, a, (&l->othr));
    while (l->val < 4096) {
        printf("Thread b (%d)\n", l->val++);
        l->val++;
        GGT_YIELD();
    }

    GGT_JOIN(l->othr);

    GGT_LONGJMP(jb);

    GGT_END();
}

int main() {
    ggt_thread_list_t list;
    ggt_thread_t thrA, thrB;
    GGT_INIT(list);
    GGT_SPAWN(list, thrA, a, (&thrA));
    GGT_SPAWN(list, thrB, b, (&thrB));
    ggtRun(&list);
    GGT_FREE(list);
    return 0;
}
