#include "ggt/green.h"

#if GGT_SUPP_THREADS
void ggggtGreenFreeThr(ggt_thread_list_t *list) {
    ggt_native_sem_destroy(list->lock);
}
#endif
