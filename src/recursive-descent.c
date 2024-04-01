#include <lam.h>
#include <eval.h>
#include <recursive-descent.h>

const Lterm* lam_parse_expression(RecDescCtx* ctx) ;
void eval_print(const Lterm t[static 1]) ;


//// Lterm values that denote errors:
///
// An lam error that implies a bug and shold not happen
const Lterm* LamInternalError = &(Lterm){
    .var={.name={.s="LamInternalError"}}
};
// A parse that faild, but the parser should continiue trying other cases
const Lterm* NotParse = &(Lterm){.var={.name={.s="NotParse"}}};
// A snyntax error
const Lterm* SyntaxError = &(Lterm){.var={.name={.s="SyntaxError"}}};
////


bool lam_parse_term_failed(const Lterm* t) {
    return !t || t == NotParse || t == SyntaxError
        || t == LamInternalError;
}

bool lam_parse_error(const Lterm* t) {
    return !t || t == SyntaxError || t == LamInternalError;
}

bool lam_parse_tk_match(RecDescCtx* ctx, LamTokenTag t) {
    return ctx->last == t;
}

LamTokenTag lam_parse_tk_next(RecDescCtx* ctx) {
    if (!ctx->unget) {
        ctx->last = lam_scan_next(&ctx->buf);
    }
    ctx->unget = false;
    return ctx->last;
}

bool lam_stmt_is_end(LamTokenTag t) {
    return t == LEof || t == LEol || t == LSemicolon;
}

bool lam_parse_tk_next_is_end(RecDescCtx* ctx) {
    lam_parse_tk_next(ctx);
    return lam_stmt_is_end(ctx->last);
}

int lam_parse_tk_unget(RecDescCtx* ctx) {
    if (ctx->unget) {
        puts("Should not undo twice, aborting");
        exit(1);// should return error
    }
    ctx->unget = true;
    //TODO: check usage
    return 0;
}

bool lam_parse_tk_next_match(RecDescCtx* ctx, LamTokenTag t) {
    lam_parse_tk_next(ctx);
    return lam_parse_tk_match(ctx, t);
}

bool lam_parse_tk_next_match_or_unget(RecDescCtx* ctx, LamTokenTag t) {
    lam_parse_tk_next(ctx);
    if (lam_parse_tk_match(ctx, t)) {
        return true;
    }
    lam_parse_tk_unget(ctx);
    return false;
}

Lstr lam_parse_tk_dup_kw(RecDescCtx* ctx) {
    return lam_strndup(ctx->buf.s, ctx->buf.len);
}


////
/// Parser functions
///

const Lterm* lam_parse_neither_lnorapp(RecDescCtx* ctx) {
    if (!ctx) return LamInternalError;
    if (lam_parse_tk_next_match_or_unget(ctx, LLparen)) {
        const Lterm* expr = lam_parse_expression(ctx);
        if (lam_parse_term_failed(expr)) { return SyntaxError; }
        if (lam_parse_tk_next_match_or_unget(ctx, LRparen)) {
            return expr; 
        }
        return SyntaxError;
    } else if (lam_parse_tk_next_match_or_unget(ctx, LVar)) {
        return lam_new_var(lam_parse_tk_dup_kw(ctx));//TODO!
    } else if (lam_parse_tk_next_match_or_unget(ctx, LName)) {
        puts("nme");
        return lam_new_var(lam_parse_tk_dup_kw(ctx));//TODO!
    }
    return NotParse;
}

const Lterm* lam_parse_not_lambda(RecDescCtx* ctx) {
    if (!ctx) return LamInternalError;
    const Lterm* app = lam_parse_neither_lnorapp(ctx);
    if (lam_parse_term_failed(app)) { return SyntaxError; }
    for(;;) {
        const Lterm* p = lam_parse_neither_lnorapp(ctx);
        if (p == NotParse) { break; }
        if (lam_parse_error(p)) { return p; }
        app = lam_new_app(app, p);
    }
    return app;
}

const Lterm* lam_parse_lambda(RecDescCtx* ctx) {
    if (!ctx) return LamInternalError;
    if (!lam_parse_tk_next_match_or_unget(ctx, LLambda)) {
        return NotParse;
    }
    if (!lam_parse_tk_next_match(ctx, LVar)) { return SyntaxError; }
    Lstr v = lam_parse_tk_dup_kw(ctx);
    if (!lam_parse_tk_next_match(ctx, LDot)) { return SyntaxError; }
    const Lterm* expr = lam_parse_expression(ctx);
    if (lam_parse_term_failed(expr)) { return SyntaxError; }
    return lam_new_abs(v, expr);
}

const Lterm* lam_parse_expression(RecDescCtx* ctx) {
    if (!ctx) return LamInternalError;
    const Lterm* t =  lam_parse_lambda(ctx);
    if (lam_parse_error(t)) { return t; }
    if (t != NotParse) { return t; }
    return lam_parse_not_lambda(ctx);
}


const Lterm* lam_parse_stmt_set(RecDescCtx* ctx) {
    if (!ctx) return LamInternalError;
    if (!lam_parse_tk_next_match_or_unget(ctx, LSet)) { return NotParse; }
    if (!lam_parse_tk_next_match_or_unget(ctx, LVar)) {
        return SyntaxError;
    }
    //TODO: use this Lstr v = lam_parse_tk_dup_kw(ctx);
    if (!lam_parse_tk_next_match_or_unget(ctx, LEquals)) {
        return SyntaxError;
    }
    const Lterm* expr = lam_parse_expression(ctx);
    if (lam_parse_term_failed(expr)) { return SyntaxError; }
    if (!lam_parse_tk_next_is_end(ctx)) {
        lam_parse_tk_unget(ctx);
        return SyntaxError;
    }
    return expr;
}

void lam_parse_stmts() {
    for (;;) {
        RecDescCtx ctx = {0};
        if (lam_parse_tk_next_match_or_unget(&ctx, LEof)) { return; }
        if (lam_parse_tk_next_is_end(&ctx)) { continue; }
        lam_parse_tk_unget(&ctx);

        const Lterm* set_stmt = lam_parse_stmt_set(&ctx);
        if (lam_parse_error(set_stmt)) {
            puts("Error parsing expression");
            continue;
        } else if (set_stmt != NotParse) {
            eval_print(set_stmt);
            continue;
        }


        const Lterm* t = lam_parse_expression(&ctx);
        if (lam_parse_error(set_stmt)) {
            puts("Error parsing expression");
        } else if (t == NotParse) {
            puts("lam internal error: should not happen?");
        } else if (!lam_parse_tk_next_is_end(&ctx)) {
            puts("Error parsing expression");
        } else {
            eval_print(t);
        }
    }
}


void eval_print(const Lterm t[static 1]) {
    EvalCtx ctx = {.len0=lam_term_len(t)};
    const Lterm* v = lam_eval_with_ctx(t, &ctx);
    if (ctx.fail) {
        printf("eval error: %s\nterm: '", ctx.msg);
        lam_print_term_less_paren(t);
        puts("'");
    } else if (!v) {
        printf("Lam internal error");
    } else {
        lam_print_term_less_paren(v);
    }
    puts("");
}
