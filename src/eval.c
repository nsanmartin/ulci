#include <eval.h>


Lterm* lam_eval_app(const Lapp app[static 1]) {
    Lstr x = app->fun->abs.vname;
    Lterm* body = app->fun->abs.body;
    Lterm* s = app->param;

    Lterm* r = lam_substitute(body, x, s);
    return r;
}


Lterm* lam_eval_(const Lterm t[static 1]) {
    if (lam_normal_form(t)) { return lam_clone(t); }
    switch(t->tag) {
        // case Lvartag: return lam_clone(t); 
        case Labstag: {
            Lterm* r = lam_new_abs(t->abs.vname, lam_eval(t->abs.body));
            return r;
        }
        case Lapptag: {
            Lterm* r = lam_new_app(lam_eval(t->app.fun), lam_eval(t->app.param));
            if (r->app.fun->tag == Labstag) {
                r = lam_eval_app(&r->app);
            }
            return lam_eval(r);
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}


const Lterm* lam_eval_with_ctx(const Lterm* t, EvalCtx* ctx) {
    if (!ctx) { return lam_internal_error(); }
    if (lam_invalid_term(t)) { return t; }
    if (lam_term_len(t) >= ctx->len0 && ctx->depth > 8) {
        return lam_not_reducing();
    }
    if (ctx->depth > 124000) { return lam_too_many_reductions(); }

    if (lam_normal_form(t)) { return lam_clone(t); }
    ctx->depth += 1;

    switch(t->tag) {
        // case Lvartag: return lam_clone(t); 
        case Labstag: {
            const Lterm* body = lam_eval_with_ctx(t->abs.body, ctx);
            if (!body) return 0x0;
            return lam_new_abs(t->abs.vname, body);
        }
        case Lapptag: {
            const Lterm* f = lam_eval_with_ctx(t->app.fun, ctx);
            if (!f) return 0x0;
            const Lterm* arg = lam_eval_with_ctx(t->app.param, ctx);
            if (!arg) return 0x0;
            const Lterm* r = lam_new_app(f, arg);
            if (!r) { return 0x0; }
            if (r->app.fun->tag == Labstag) {
                r = lam_eval_app(&r->app);
            }
            return lam_eval_with_ctx(r, ctx);
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}

Lterm* lam_eval(const Lterm t[static 1]) {
    //TODO: use eval with ctx
    return lam_eval_(t);
}

////
//// StmtReadCallbacks:
///

void eval_print(const Lterm t[static 1], void* ignore) {
    (void) ignore;
    void (*on_parse)(const Lterm t[static 1]) = lam_print_term_less_paren;

    EvalCtx ctx = {.len0=lam_term_len(t)};
    const Lterm* v = lam_eval_with_ctx(t, &ctx);
    if (lam_not_reducing()) {
        printf("eval error: %s\nterm: '", "term is not reducing");
        on_parse(t);
        puts("'");
    } else if (lam_too_many_reductions()) {
        printf("eval error: %s\nterm: '", "eval stack too large");
        on_parse(t);
        puts("'");
    } else if (lam_internal_error()) {
        puts("Lam eval internal error");
    } else {
        on_parse(v);
    }
    puts("");
}

void eval_to_list(Lterm* t[static 1], void* acum) {
    EvalCtx ctx = {.len0=lam_term_len(*t)};
    const Lterm* v = lam_eval_with_ctx(*t, &ctx);
    if (acum) {

        LtermList* node = lam_malloc(sizeof(LtermList));
        if (!node) { puts("lam malloc error"); exit(EXIT_FAILURE); }
        *node = (LtermList){ .next=NULL,.t=v };

        ToListCallbackAcum* a = acum; 
        if (!a->first) {
            // base case
            a->first = node;
            a->last = node;
        } else {
            a->last->next = node;
            a->last = a->last->next;
        }
    } else {
        puts("eval to list error"); exit(EXIT_FAILURE);
    }
}

/*
 * StmtReadCallbacks */
