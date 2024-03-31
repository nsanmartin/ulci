#ifndef __LAM_EVAL_H_
#define __LAM_EVAL_H_

#include <lam.h>

typedef struct {
    unsigned depth;
    unsigned len0;
    Lterm* t0;
    const char* msg;
    bool fail;
} EvalCtx;

Lterm* lam_eval(const Lterm t[static 1]) ;
const Lterm* lam_eval_with_ctx(const Lterm* t, EvalCtx* ctx) ;
unsigned lam_term_len(const Lterm* t) ;

#endif
