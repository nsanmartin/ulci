#include <lam.h>
#include <recursive-descent.h>

const Lterm _NotParseTermMem = {0};
const Lterm* NotParseTerm = &_NotParseTermMem;

bool lam_stmt_is_end(LamTokenTag t) {
    return t == LEof || t == LEol || LSemicolon;
}

const Lterm* lam_rec_descent(RecDescCtx* ctx) {
    if (!ctx) return 0x0;
    return NotParseTerm;
}

const Lterm* lam_parse_expression(RecDescCtx* ctx) {
    if (!ctx) return 0x0;
    return NotParseTerm;
}

typedef enum { LAssignTag, LExprTag, LNotParseTag } LStmtTag;
typedef struct {
    const Lterm* t;
    Lstr v;
} Lassign;

typedef struct {
    LStmtTag tag;
    union {
        const Lterm* expr;
        Lassign assign;
    };

} LStmt;

LStmt lam_parse_stmt(RecDescCtx* ctx) {
    LStmt rv = {.tag=LNotParseTag};
    
    LamTokenTag tk = lam_scan_next(&ctx->buf);
    if (tk == LSet) {
        tk = lam_scan_next(&ctx->buf);
        if (tk != LVar) { return rv; }
        tk = lam_scan_next(&ctx->buf);
        if (tk != LEquals) { return rv; }
        const Lterm* expr = lam_parse_expression(ctx);
        if (expr == NotParseTerm) { return rv; }
        tk = lam_scan_next(&ctx->buf);
        if (!lam_stmt_is_end(tk)) { return rv; }
        rv.expr = expr;
        return rv;
    }
    //TODO: parse expression
    return rv;
}
