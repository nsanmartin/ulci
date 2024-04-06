#include <lam.h>

Lterm* lam_red_subst(Lterm t[static 1], Lstr x, Lterm s[static 1]) {
    switch(t->tag) {
        case Lvartag: {
            if (lam_str_eq(t->var.name, x)) { return lam_clone(s); }
            else { return t; }                              // free t?
        }
        case Labstag: {
            if (lam_is_var_free_in(t,x)) {
                if (lam_is_var_free_in(s, t->abs.vname)) {
                    Lstr fresh_name = lam_get_fresh_var_name(t);
                    if (lam_str_null(fresh_name)) { return 0x0; }
                    lam_rename_var(t->abs.body, t->abs.vname, fresh_name);
					t->abs.vname = fresh_name;
                }

                t->abs.body = lam_red_subst(t->abs.body, x, s);
                if (!t->abs.body) { return 0x0; }
                return t;
            } else { //x is captured by \x
                return t;
            }
        }
        case Lapptag: {
            t->app.fun = lam_red_subst(t->app.fun, x, s);
            if (!t->app.fun) { return 0x0; }
            t->app.param = lam_red_subst(t->app.param, x, s);
            if (!t->app.param) {  return 0x0; }
            return t;
        }
        default: return (Lterm*)LamInternalError ;
    }
}

Lterm* lam_reduce_abs(Lterm t[static 1]) {
    return t;
}

Lterm* lam_reduce_step(Lterm t[static 1]) {
    if (!t) { return (Lterm*)LamInternalError; }
    switch (t->tag) {
        case Lvartag: return t;
        case Labstag: {
            t->abs.body = lam_reduce_step(t->abs.body);
            if (lam_invalid_term(t->abs.body)) { return t; }
            return t;
          };
        case Lapptag: {
            if (lam_normal_form(t->app.fun) && lam_normal_form(t->app.param)) {
                if (t->app.fun->tag == Labstag) {
                    return lam_red_subst(
                        t->app.fun->abs.body,
                        t->app.fun->abs.vname,
                        t->app.param
                    );
                }
                return t;
            } else if (lam_normal_form(t->app.fun)) {
                t->app.param = lam_reduce_step(t->app.param);
                return t;
            } else {
                t->app.fun = lam_reduce_step(t->app.fun);
                return t;
            }
        }
        default: return (Lterm*)LamInternalError;
    }
}

void lam_term_log(const Lterm* t, unsigned nreds) {
    printf(
        "len: %d, height: %d, nreds: %d\n",
        lam_term_len(t),
        lam_term_height(t),
        nreds
    );
}

Lterm* lam_reduce(Lterm* t) {
    unsigned max_reductions = 10000;
    unsigned nreds = 0;
    for (
        ; nreds < max_reductions && !lam_normal_form(t)
        ; ++nreds, t = lam_reduce_step(t)
    ) {
        lam_term_log(t, nreds);
    }
    if (nreds == max_reductions) {
        puts("too many reductions");
        return (Lterm*)EvalStackTooLarge;
    }
    return t;
}

