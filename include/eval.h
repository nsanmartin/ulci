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

typedef struct {
    void* rv;
} EvalRes;


struct LtermList_;
typedef struct LtermList_ LtermList;
struct LtermList_ {
    const Lterm* t;
    LtermList* next;
};

Lterm* lam_eval(const Lterm t[static 1]) ;
const Lterm* lam_eval_with_ctx(const Lterm* t, EvalCtx* ctx) ;
unsigned lam_term_len(const Lterm* t) ;
void eval_print(const Lterm t[static 1], void*);
void eval_to_list(const Lterm t[static 1], void* acum);

#endif
