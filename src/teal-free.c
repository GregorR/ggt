#include "ggt/native/detect-ctx.h"

#if GGGGT_SUPP_CTX_SWITCH
#include "ggt/teal.h"

#if GGT_SUPP_THREADS
void ggggtTealFreeThr(ggt_thread_list_t *list) {
    ggt_native_sem_destroy(list->lock);
}
#endif

#endif
