#include <eval.h>

unsigned lam_term_len(const Lterm* t) {
    switch (t->tag) {
        case Lvartag: return 1;
        case Labstag: return 3 + lam_term_len(t->abs.body);
        case Lapptag: {
            unsigned llen = lam_term_len(t->app.fun);
            unsigned rlen = lam_term_len(t->app.param);
            return rlen + llen;
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}

unsigned lam_term_height(const Lterm* t) {
    switch (t->tag) {
        case Lvartag: return 1;
        case Labstag: return 1 + lam_term_height(t->abs.body);
        case Lapptag: {
            unsigned lheight = lam_term_height(t->app.fun);
            unsigned rheight = lam_term_height(t->app.param);
            return 1 + lheight > rheight ? lheight : rheight;
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}

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
    if (!ctx) { return 0x0; }
    if (!t) { ctx->fail = true; return 0x0; }
    if (lam_term_len(t) >= ctx->len0) {
        ctx->msg = "term is not reducing.";
        ctx->fail = true;
        return t;
    }

    if (ctx->depth > 1024) {
        ctx->msg = "eval stack too large";
        ctx->fail = true;
        return t;
    }
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

