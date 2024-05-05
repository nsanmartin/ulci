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

Lterm* lam_syntax_error(const char* msg, RecDescCtx* ctx);

typedef struct {
    void (*callback)(Lterm*t[static 1], void*);
    void* acum;
} StmtReadCallback;

void lam_parse_stmts(StmtReadCallback* on_stmt_read) ;

static inline bool lam_parse_term_failed(const Lterm* t) {
    return !t || t->tag >= LamtagCount || (t->tag == Lerrtag
        && (
            t->err.tag == LNotParseTag
            || t->err.tag == LSyntaxErrorTag
            || t->err.tag == LInternalErrorTag
        )
    );
}

/*
 * An "exception" during parsing includes s syntax error (the user proviedd an invalid input)
 * or an internal lam error (NULL ptr or LInternalError).
 */
static inline bool lam_parse_term_exception(const Lterm* t) {
    return !t || t->tag >= LamtagCount || (t->tag == Lerrtag && (t->err.tag == LSyntaxErrorTag || t->err.tag == LInternalErrorTag));
}

static inline bool lam_is_not_parse(const Lterm* t) {
    return t && t->tag == Lerrtag && t->err.tag == LNotParseTag;
}

static inline bool lam_is_syntax_error(const Lterm* t) {
    return t && t->tag == Lerrtag && t->err.tag == LSyntaxErrorTag;
}

static inline bool lam_is_internal_error(const Lterm* t) {
    return !t || t->tag >= LamtagCount || (t->tag == Lerrtag && t->err.tag == LInternalErrorTag);
}

static inline bool lam_parse_tk_match(RecDescCtx* ctx, LamTokenTag t) {
    return ctx->last == t;
}

static inline bool lam_stmt_is_end(LamTokenTag t) {
    return t == LEof || t == LEol || t == LSemicolon;
}

static inline Lstr lam_parse_tk_dup_kw(RecDescCtx* ctx) {
    return lam_lstrndup(ctx->buf.s, ctx->buf.len);
}

static inline Lstr lam_parse_tk_kw_view(RecDescCtx* ctx) {
    return lam_strn_view(ctx->buf.s, ctx->buf.len);
}

#endif
