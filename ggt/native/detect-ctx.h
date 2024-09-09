#ifndef GGGGT_NATIVE_DETECT_CTX_H
#define GGGGT_NATIVE_DETECT_CTX_H

#define GGGGT_SUPP_CTX_SWITCH_X86_64 1

#if defined(__GNUC__)
#if defined(__x86_64__)
#define GGGGT_SUPP_CTX_SWITCH GGGGT_SUPP_CTX_SWITCH_X86_64
#endif
#endif

#ifndef GGGGT_SUPP_CTX_SWITCH
#define GGGGT_SUPP_CTX_SWITCH 0
#endif

#endif
