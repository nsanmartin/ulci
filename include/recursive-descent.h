#ifndef __LAM_REC_DESCENT_H_
#define __LAM_REC_DESCENT_H_
#include <lexer.h>

typedef struct {
    LamKeywordBuf buf;
    bool fail;
    LamTokenTag last;
    bool unget;
} RecDescCtx;

void lam_parse_stmts() ;
#endif
