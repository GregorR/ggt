#include "ggt/native/detect-ctx.h"

#if GGGGT_SUPP_CTX_SWITCH
#include "ggt/teal.h"

#if GGT_COMP_THREADS == GGT_SUPP_THREADS

void GGGGT_THR(ggggtTealInit)(ggt_thread_list_t *list) {
    list->next = NULL;
    *list->cleanup = NULL;
#if GGT_SUPP_THREADS
    ggt_native_sem_init(list->lock, 1);
#endif
}

#endif

#endif
