#ifndef __LAM_REC_DESCENT_H_
#define __LAM_REC_DESCENT_H_

#include <lexer.h>

#include <stdlib.h>


typedef struct {
    LamKeywordBuf buf;
    bool fail;
    LamTokenTag last;
    bool unget;
} RecDescCtx;

typedef struct {
    void (*callback)(const Lterm[static 1], void*);
    void* acum;
} StmtReadCallback;
void lam_parse_stmts(StmtReadCallback* on_stmt_read) ;
#endif
