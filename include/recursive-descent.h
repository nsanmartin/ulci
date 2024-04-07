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
    void (*callback)(Lterm[static 1], void*);
    void* acum;
} StmtReadCallback;

void lam_parse_stmts(StmtReadCallback* on_stmt_read) ;

static inline bool lam_parse_term_failed(const Lterm* t) {
    return !t
        || t == NotParse
        || t == SyntaxError
        || t == LamInternalError;
}

static inline bool lam_parse_error(const Lterm* t) {
    return !t || t == SyntaxError || t == LamInternalError;
}

static inline bool lam_parse_tk_match(RecDescCtx* ctx, LamTokenTag t) {
    return ctx->last == t;
}

static inline bool lam_stmt_is_end(LamTokenTag t) {
    return t == LEof || t == LEol || t == LSemicolon;
}

static inline Lstr lam_parse_tk_dup_kw(RecDescCtx* ctx) {
    return lam_strndup(ctx->buf.s, ctx->buf.len);
}

static inline Lstr lam_parse_tk_kw_view(RecDescCtx* ctx) {
    return lam_strndup(ctx->buf.s, ctx->buf.len);
}

#endif
