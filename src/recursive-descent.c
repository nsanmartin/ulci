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
    if (!ctx) return (Lterm*)LamInternalError;
    if (lam_parse_tk_next_match_or_unget(ctx, LLparen)) {
        Lterm* expr = lam_parse_expression(ctx);
        if (lam_parse_term_failed(expr)) { return (Lterm*)SyntaxError; }
        if (lam_parse_tk_next_match_or_unget(ctx, LRparen)) {
            return expr; 
        }
        return (Lterm*)SyntaxError;
    } else if (lam_parse_tk_next_match_or_unget(ctx, LVar)) {
        return lam_var(lam_parse_tk_dup_kw(ctx));
    } else if (lam_parse_tk_next_match_or_unget(ctx, LName)) {
        return lam_symbols_search_clone(lam_parse_tk_kw_view(ctx));
    }
    return (Lterm*)NotParse;
}

Lterm* lam_parse_not_lambda(RecDescCtx* ctx) {
    if (!ctx) return (Lterm*)LamInternalError;
    Lterm* app = lam_parse_neither_lnorapp(ctx);
    if (lam_parse_term_failed(app)) {
        //lam_free_term(app); // if failed should have been freed already
        return (Lterm*)SyntaxError;
    }
    for(;;) {
        Lterm* p = lam_parse_neither_lnorapp(ctx);
        if (p == NotParse) { break; }
        if (lam_parse_error(p)) { return p; }
        app = lam_app(app, p);
    }
    return app;
}

Lterm* lam_parse_lambda(RecDescCtx* ctx) {
    if (!ctx) return (Lterm*)LamInternalError;
    if (!lam_parse_tk_next_match_or_unget(ctx, LLambda)) {
        return (Lterm*)NotParse;
    }
    if (!lam_parse_tk_next_match(ctx, LVar)) { return (Lterm*)SyntaxError; }
    Lstr v = lam_parse_tk_dup_kw(ctx);
    if (!lam_parse_tk_next_match(ctx, LDot)) {
        lam_free((void*)v.s);
        return (Lterm*)SyntaxError;
    }
    Lterm* expr = lam_parse_expression(ctx);
    if (lam_parse_term_failed(expr)) {
        lam_free((void*)v.s);
        return (Lterm*)SyntaxError;
    }
    return lam_abs(v, expr);

}

Lterm* lam_parse_expression(RecDescCtx* ctx) {
    if (!ctx) return (Lterm*)LamInternalError;
    Lterm* t =  lam_parse_lambda(ctx);
    if (lam_parse_error(t)) { return t; }
    if (t != NotParse) { return t; }
    return lam_parse_not_lambda(ctx);
}


Lterm* lam_parse_stmt_set(RecDescCtx* ctx) {
    if (!ctx) return (Lterm*)LamInternalError;
    if (!lam_parse_tk_next_match_or_unget(ctx, LSet)) { return (Lterm*)NotParse; }
    if (!lam_parse_tk_next_match_or_unget(ctx, LVar)) {
        return (Lterm*)SyntaxError;
    }
    Lstr v = lam_parse_tk_dup_kw(ctx);
    if (lam_str_null(v)) { return 0x0; }
    if (!lam_parse_tk_next_match_or_unget(ctx, LEquals)) {
        free((void*)v.s);
        return (Lterm*)SyntaxError;
    }
    Lterm* expr = lam_parse_expression(ctx);
    if (lam_parse_term_failed(expr)) {
        free((void*)v.s);
        return (Lterm*)SyntaxError;
    }
    if (!lam_parse_tk_next_is_end(ctx)) {
        free((void*)v.s);
        lam_free_term(expr);
        lam_parse_tk_unget(ctx);
        return (Lterm*)SyntaxError;
    }

    int err = lam_str_name_insert(v, (Lterm*)expr);
    if (err) {
        free((void*)v.s);
        lam_free_term(expr);
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
        if (lam_parse_tk_next_is_end(&ctx)) { continue; }
        lam_parse_tk_unget(&ctx);

        Lterm* set_expr = lam_parse_stmt_set(&ctx);
        if (set_expr == SyntaxError) {
            puts("syntax error");
            continue;
        } else if (!set_expr || set_expr == LamInternalError) {
            puts("lam internal error, aborting.");
            exit(EXIT_FAILURE);
        } else if (set_expr != NotParse) {
            //on_stmt_read->callback((Lterm*)set_expr, on_stmt_read->acum);
            lam_parse_apply_callbacks(on_stmt_read, &set_expr);
            continue;
        }


        Lterm* t = lam_parse_expression(&ctx);
        if (t == SyntaxError) {
            puts("syntax error");
        } else if (!t || t == LamInternalError) {
            puts("lam internal error, aborting.");
            exit(EXIT_FAILURE);
        } else if (t == NotParse) {
            puts("lam internal error: should not happen?");
        } else if (!lam_parse_tk_next_is_end(&ctx)) {
            puts("Error parsing expression");
        } else {
            //on_stmt_read->callback((Lterm*)t, on_stmt_read->acum);
            lam_parse_apply_callbacks(on_stmt_read, &t);
        }
    }
}

