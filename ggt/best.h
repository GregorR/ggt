#ifndef GGGGT_BEST_H
#define GGGGT_BEST_H 1

#if !defined(GGT_SUPP_NATIVE) || GGT_SUPP_NATIVE
#include "native/detect.h"
#undef GGT_SUPP_NATIVE
#define GGT_SUPP_NATIVE GGGGT_THREADS_NATIVE
#endif

#if !defined(GGT_SUPP_TEAL) || GGT_SUPP_TEAL
#include "native/detect-ctx.h"
#undef GGT_SUPP_TEAL
#define GGT_SUPP_TEAL GGGGT_SUPP_CTX_SWITCH
#endif

#if defined(GGT_FORCE_FAKE) && GGT_FORCE_FAKE
#include "fake.h"
#elif GGT_SUPP_NATIVE
#include "native.h"
#elif GGT_SUPP_TEAL
#include "teal.h"
#else
#include "green.h"
#endif

#endif
