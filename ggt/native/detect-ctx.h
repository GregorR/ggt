#ifndef GGGGT_NATIVE_DETECT_CTX_H
#define GGGGT_NATIVE_DETECT_CTX_H

#define GGGGT_SUPP_CTX_SWITCH_X86	0x001
#define GGGGT_SUPP_CTX_SWITCH_X86_64	0x002
#define GGGGT_SUPP_CTX_SWITCH_ARM       0x011
#define GGGGT_SUPP_CTX_SWITCH_ARM64     0x012

#if defined(__GNUC__)
#if defined(__x86_64__)
#define GGGGT_SUPP_CTX_SWITCH GGGGT_SUPP_CTX_SWITCH_X86_64
#elif defined(__i386__)
#define GGGGT_SUPP_CTX_SWITCH GGGGT_SUPP_CTX_SWITCH_X86
#elif defined(__aarch64__)
#define GGGGT_SUPP_CTX_SWITCH GGGGT_SUPP_CTX_SWITCH_ARM64
#elif defined(__arm__)
#define GGGGT_SUPP_CTX_SWITCH GGGGT_SUPP_CTX_SWITCH_ARM
#endif
#endif

#ifndef GGGGT_SUPP_CTX_SWITCH
#define GGGGT_SUPP_CTX_SWITCH 0
#endif

#endif
