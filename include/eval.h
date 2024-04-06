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


typedef struct LtermList {
    const Lterm* t;
    struct LtermList* next;
} LtermList;


typedef struct {
    LtermList* first;
    LtermList* last;
} ToListCallbackAcum;

Lterm* lam_eval(const Lterm t[static 1]) ;
const Lterm* lam_eval_with_ctx(const Lterm* t, EvalCtx* ctx) ;
unsigned lam_term_len(const Lterm* t) ;
void eval_print(const Lterm t[static 1], void* ignore);
void eval_to_list(Lterm t[static 1], void* acum);

#endif
