#include "ggt/green.h"

void GGGGT_THR(ggggtGreenInit)(ggt_thread_list_t *list) {
    list->next = NULL;
#if GGT_SUPP_THREADS
    ggt_native_sem_init(list->lock, 1);
#endif
}

