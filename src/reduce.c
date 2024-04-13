#include <lam.h>

//TODOP: rename (Cf. l:61)
Lterm* lam_reduce_abs(Lterm body[static 1], Lstr x, Lterm s[static 1]) {
    switch(body->tag) {
        case Lvartag: {
            if (lam_str_eq(body->var.name, x)) { 
                lam_free_term(body);
                free((void*)x.s);
                return s;
            }
            else {
                lam_free_term(s);
                free((void*)x.s);
                return body;
            }
        }
        case Labstag: {
            if (lam_is_var_free_in(body, x)) {
                if (lam_is_var_free_in(s, body->abs.vname)) {
                    //TODO: review this
                    puts("rename var! (TODO: remove me)====="); exit(1);
                    ///Lstr fresh_name = lam_get_fresh_var_name(body);
                    ///if (lam_str_null(fresh_name)) { return 0x0; }
                    ///lam_rename_var(body->abs.body, body->abs.vname, fresh_name);
					///body->abs.vname = fresh_name;
                }

                Lterm* b = lam_reduce_abs(body->abs.body, lam_lstr_dup(x), lam_clone(s));
                if (lam_invalid_term(b)) {
                    puts("DEBUG: invalid term"); 
                    lam_free(s);
                    lam_free(body);
                    free((void*)x.s);
                    return b;
                }
                free((void*)x.s);
                return b;
            } else { //x is captured by \x
                free((void*)x.s);
                lam_free_term(s);
                return body;
            }
        }
        case Lapptag: {
            Lterm* fred = lam_reduce_abs(body->app.fun, lam_lstr_dup(x), lam_clone(s));
            if (lam_invalid_term(fred)) {
                free((void*)x.s);
                lam_free_term(s);
                lam_free_term(body);
                return fred;
            }
            body->app.fun = fred;
            Lterm* pred = lam_reduce_abs(body->app.param, x, s);
            if (lam_invalid_term(pred)) {
                free((void*)x.s);
                lam_free_term(body);
                return pred;
            }
            body->app.param = pred;
            return body;
        }
        default: {
            lam_free_term(body);
            lam_free_term(s);
            free((void*)x.s);
             return (Lterm*)LamInternalError;
        };
    }
}
/*
 * This function "consumes its parameters: their heap memory is freed after this fn returns.
 */
Lterm* lam_red_subst(Lterm t[static 1], const Lstr x, Lterm s[static 1]) {
    switch(t->tag) {
        case Lvartag: {
            if (lam_str_eq(t->var.name, x)) { 
                free((void*)x.s);
                lam_free(t);
                return s;
            }
            else {
                free((void*)x.s);
                lam_free(s);
                return t;
            }
        }
        case Labstag: {
            if (lam_is_var_free_in(t,x)) {
                if (lam_is_var_free_in(s, t->abs.vname)) {
                    //TODO: review this
                    puts("rename var! (TODO: remove me)====="); exit(1);
                    Lstr fresh_name = lam_get_fresh_var_name(t);
                    if (lam_str_null(fresh_name)) { return 0x0; }
                    lam_rename_var(t->abs.body, t->abs.vname, fresh_name);
					t->abs.vname = fresh_name;
                }

                Lterm* b = lam_red_subst(t->abs.body, lam_lstr_dup(x), s);
                if (lam_invalid_term(b)) {
                    puts("DEBUG: invalid term"); 
                    lam_free(t);
                    free((void*)x.s);
                    return b;
                }
                free((void*)x.s);
                return b;
            } else { //x is captured by \x
                free((void*)x.s);
                return t;
            }
        }
        case Lapptag: {
            Lterm* fred = lam_red_subst(t->app.fun, lam_lstr_dup(x), s);
            if (lam_invalid_term(fred)) {
                free((void*)x.s);
                lam_free(t);
                return fred;
            }
            t->app.fun = fred;
            Lterm* s_clone = lam_clone(s);
            if (lam_invalid_term(s)) {
                free((void*)x.s);
                lam_free(t);
                return 0x0;
            }
            Lterm* pred = lam_red_subst(t->app.param, x, s_clone);
            if (lam_invalid_term(pred)) {
                free((void*)x.s);
                lam_free(t);
                return pred;
            }
            t->app.param = pred;
            free((void*)x.s);
            return t;
        }
        default: {
            lam_free(t);
             return (Lterm*)LamInternalError;
        };
    }
}


/*
 * This function frees the heap memory owned by t and returns t' (its reduction).
 * It should be used like:
 *  t = lam_reduce_step(t);
 *
 */
Lterm* lam_reduce_step(Lterm* t) {
    if (!t) { return (Lterm*)LamInternalError; }
    switch (t->tag) {
        case Lvartag: return t;
        case Labstag: {
            Lterm* b = lam_reduce_step(t->abs.body);
            if (lam_invalid_term(b)) {
                lam_free_term(t);
                return b;
            }
            t->abs.body = b;
            return t;
          };
        case Lapptag: {
            if (lam_normal_form(t->app.fun) && lam_normal_form(t->app.param)) {
                if (t->app.fun->tag == Labstag) {
                    Lterm* red = lam_reduce_abs(t->app.fun->abs.body, t->app.fun->abs.vname, t->app.param);
                    free(t->app.fun);
                    free(t);
                    return red;
                }
                return t;
            } else if (lam_normal_form(t->app.fun)) {
                Lterm* p = lam_reduce_step(t->app.param);
                if (lam_invalid_term(p)) {
                    lam_free(t);
                    return p;
                }
                t->app.param = p;
                return t;
            } else {
                Lterm* f = lam_reduce_step(t->app.fun);
                if (lam_invalid_term(f)) {
                    lam_free(t);
                    return f;
                }
                t->app.fun = f;
                return t;
            }
        }
        default: {
            lam_free_term(t);
            return (Lterm*)LamInternalError;
         };
    }
}

void lam_term_log(const Lterm* t, unsigned nreds) {
    printf(
        "len: %d, height: %d, nreds: %d./\n",
        lam_term_len(t),
        lam_term_height(t),
        nreds
    );
    //lam_print_term_less_paren(t);
    //puts("/");
}

Lterm* lam_reduce(Lterm* t) {
    unsigned max_reductions = 10000;
    unsigned nreds = 0;
    for (
        ; nreds < max_reductions && !lam_normal_form(t)
        ; ++nreds, t = lam_reduce_step(t)
    ) {
        if (nreds % 999 == 0) { lam_term_log(t, nreds); }
    }
    if (nreds == max_reductions) {
        puts("too many reductions");
        return (Lterm*)EvalStackTooLarge;
    }
    return t;
}

