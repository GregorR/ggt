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
 * Example code that generates millions of threads, and often fails if native
 * threads are used.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#ifndef GGT_SUPP_NATIVE
#define GGT_SUPP_NATIVE 0
#endif
#include "../ggt/best.h"

#define TOTAL_SIZE (1024*1024)

GGT_E(sort, (ggt_thread_t *thr, int *arr, size_t len), {
    int *arr;
    size_t len;
    size_t half;
    ggt_thread_t left;
    ggt_thread_t right;
}, {
    l->arr = arr;
    l->len = len;
}) {
    int *join, *ll, *rl;
    size_t i, ls, rs;

    if (l->len <= 1)
        GGT_RETURN();

    l->half = l->len / 2;
    GGT_SPAWN(*thr, l->left, sort, (&l->left, l->arr, l->half));
    GGT_SPAWN(*thr, l->right, sort, (
        &l->right, l->arr + l->half, l->len - l->half
    ));

    GGT_JOIN(l->left);

    GGT_JOIN(l->right);

    join = malloc(sizeof(int) * l->len);
    i = 0;
    ll = l->arr;
    rl = l->arr + l->half;
    ls = l->half;
    rs = l->len - l->half;
    while (ls && rs) {
        if (ll[0] < rl[0]) {
            join[i++] = ll[0];
            ll++;
            ls--;
        } else {
            join[i++] = rl[0];
            rl++;
            rs--;
        }
    }
    while (ls) {
        join[i++] = ll[0];
        ll++;
        ls--;
    }
    while (rs) {
        join[i++] = rl[0];
        rl++;
        rs--;
    }
    memcpy(l->arr, join, sizeof(int) * l->len);
    free(join);

    GGT_END();
}

int main() {
    int *arr;
    size_t i;
    ggt_thread_list_t list;
    ggt_thread_t thr;

    arr = malloc(sizeof(int) * TOTAL_SIZE);
    for (i = 0; i < TOTAL_SIZE; i++)
        arr[i] = rand();

    GGT_INIT(list);
    GGT_SPAWN(list, thr, sort, (&thr, arr, TOTAL_SIZE));
    ggtRun(&list);
    GGT_FREE(list);

    return 0;
}
