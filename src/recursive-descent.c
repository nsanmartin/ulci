#include <lam.h>
#include <eval.h>
#include <recursive-descent.h>
#include <symbols.h>

Lterm* lam_parse_expression(RecDescCtx* ctx) ;


LamTokenTag lam_parse_tk_next(RecDescCtx* ctx) {
    if (!ctx->unget) {
        ctx->last = lam_scan_next(&ctx->buf);
    }
    ctx->unget = false;
    return ctx->last;
}

bool lam_parse_tk_next_is_end(RecDescCtx* ctx) {
    lam_parse_tk_next(ctx);
    return lam_stmt_is_end(ctx->last);
}


int lam_parse_tk_unget(RecDescCtx* ctx) {
    if (ctx->unget) {
        puts("Lam internal error: should not unget twice, aborting");
        exit(EXIT_FAILURE);// should return error instead?
    }
    ctx->unget = true;
    return 0;
}

bool lam_parse_tk_next_is_end_or_unget(RecDescCtx* ctx) {
    if (lam_parse_tk_next_is_end(ctx)) {
        return true;
    }
    lam_parse_tk_unget(ctx);
    return false;
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


////
/// Parser functions
///

Lterm* lam_parse_neither_lnorapp(RecDescCtx* ctx) {
    if (!ctx) return lam_internal_error();
    if (lam_parse_tk_next_match_or_unget(ctx, LLparen)) {
        Lterm* expr = lam_parse_expression(ctx);
        if (lam_parse_term_failed(expr)) {
            return expr; 
        }
        if (lam_parse_tk_next_match_or_unget(ctx, LRparen)) {
            return expr; 
        }
        lam_free_term(expr);
        return lam_syntax_error("Expecting LRparen", ctx);
    } else if (lam_parse_tk_next_match_or_unget(ctx, LVar)) {
        return lam_var(lam_parse_tk_dup_kw(ctx));
    } else if (lam_parse_tk_next_match_or_unget(ctx, LName)) {
        return lam_symbols_search_clone(lam_parse_tk_kw_view(ctx));
    }

    return lam_not_parse("expecting a Var or a Name", ctx);
}

Lterm* lam_parse_not_lambda(RecDescCtx* ctx) {
    if (!ctx) return lam_internal_error();
    Lterm* app = lam_parse_neither_lnorapp(ctx);
    if (lam_parse_term_failed(app)) { return app; }
    for(;;) {
        Lterm* p = lam_parse_neither_lnorapp(ctx);
        if (lam_is_not_parse(p)) {
            lam_free_term(p);
            return app;
        }
        if (lam_parse_term_failed(p)) {
            lam_free_term(app);
            return p;
        }
        app = lam_app(app, p);
    }
}

Lterm* lam_parse_lambda(RecDescCtx* ctx) {
    if (!ctx) return lam_internal_error();
    if (!lam_parse_tk_next_match_or_unget(ctx, LLambda)) {
        return lam_not_parse("Expecting Lambda", ctx);
    }
    if (!lam_parse_tk_next_match(ctx, LVar)) { return lam_syntax_error("Expecting Var", ctx); }
    Lstr v = lam_parse_tk_dup_kw(ctx);
    if (!lam_parse_tk_next_match(ctx, LDot)) {
        lam_free((void*)v.s);
        return lam_syntax_error("Expecting a Dot", ctx);
    }
    Lterm* expr = lam_parse_expression(ctx);
    if (lam_parse_term_failed(expr)) {
        lam_free((void*)v.s);
        return expr;
    }
    return lam_abs(v, expr);
}

Lterm* lam_parse_expression(RecDescCtx* ctx) {
    if (!ctx) return lam_internal_error();
    Lterm* t =  lam_parse_lambda(ctx);
    if (lam_parse_term_exception(t)) { return t; }
    if (!lam_is_not_parse(t)) { return t; }
    lam_free_term(t);
    return lam_parse_not_lambda(ctx);
}


Lterm* lam_parse_stmt_set(RecDescCtx* ctx) {
    if (!ctx) return lam_internal_error();
    if (!lam_parse_tk_next_match_or_unget(ctx, LSet)) { return lam_not_parse("expecting Set", ctx); }
    if (!lam_parse_tk_next_match_or_unget(ctx, LVar)) {
        return lam_syntax_error("Expecting a Var", ctx);
    }
    Lstr v = lam_parse_tk_dup_kw(ctx);
    if (lam_str_null(v)) { return 0x0; }
    if (!lam_parse_tk_next_match_or_unget(ctx, LEquals)) {
        lam_free((void*)v.s);
        return lam_syntax_error("Expecting Equals", ctx);
    }
    Lterm* expr = lam_parse_expression(ctx);
    if (lam_parse_term_failed(expr)) {
        lam_free((void*)v.s);
        return expr;
    }
    if (!lam_parse_tk_next_is_end(ctx)) {
        lam_free((void*)v.s);
        lam_free_term(expr);
        lam_parse_tk_unget(ctx);
        return lam_syntax_error("Expecting End of stmt", ctx);
    }

    int err = lam_str_name_insert(v, (Lterm*)expr);
    if (err) {
        lam_free((void*)v.s);
        lam_free_term(expr);
        return lam_internal_error();
    }
    return lam_clone(expr);
}

void lam_parse_apply_callbacks(StmtReadCallback* callbackp, Lterm* t[static 1]) {
    if (!callbackp) { puts("Lam error: lam_parse_stmt NULL callback, aborting."); exit(EXIT_FAILURE); }
    for (; callbackp->callback; ++callbackp) {
            callbackp->callback(t, callbackp->acum);
    }
}

void lam_parse_stmts(StmtReadCallback* on_stmt_read) {
    for (;;) {
        RecDescCtx ctx = {0};
        if (lam_parse_tk_next_match_or_unget(&ctx, LEof)) { return; }
        if (lam_parse_tk_next_is_end_or_unget(&ctx)) { continue; }

        Lterm* t = lam_parse_stmt_set(&ctx);

        if (lam_is_not_parse(t)) {
            lam_free_term(t);
            t = lam_parse_expression(&ctx);

            if (ctx.last !=  LErrorInputTooLarge 
                && !(lam_parse_tk_next_is_end_or_unget(&ctx) || lam_parse_tk_next_match_or_unget(&ctx, LEof))) {
                lam_free_term(t);
                t = lam_syntax_error("expecting End", &ctx);
            }
        } 

        if (ctx.last ==  LErrorInputTooLarge) {
            lam_free_term(t);
            t = lam_lexical_error("expression too large");
        }
        lam_parse_apply_callbacks(on_stmt_read, &t);
    }
}

