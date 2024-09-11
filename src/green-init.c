#include "ggt/green.h"

#if GGT_COMP_THREADS == GGT_SUPP_THREADS

void GGGGT_THR(ggggtGreenInit)(ggt_thread_list_t *list) {
    list->next = NULL;
#if GGT_SUPP_THREADS
    ggt_native_sem_init(list->lock, 1);
#endif
}

#endif
