#include <limits.h>
#include <lam.h>

Lterm* lam_subst_inplace(Lterm body[static 1], Lstr x, Lterm s[static 1]) {
    switch(body->tag) {
        case Lvartag: {
            if (lam_str_eq(body->var.name, x)) { 
                lam_free_term(body);
                lam_free((void*)x.s);
                return s;
            }
            else {
                lam_free_term(s);
                lam_free((void*)x.s);
                return body;
            }
        }
        case Labstag: {
            if (lam_is_var_free_in(body, x)) {
                if (lam_is_var_free_in(s, body->abs.vname)) {
                    //TODO: review this
                    Lstr fresh_name = lam_get_fresh_var_name(body);
                    printf("fresh name: %s\n", fresh_name.s);
                    if (lam_str_null(fresh_name)) {
                        lam_free_term(body);
                        lam_free_term(s);
                        lam_free((void*)x.s);
                        return 0x0;
                    }
                    if (lam_rename_var_in_place(body->abs.body, body->abs.vname, fresh_name)) {
                        lam_free_term(body);
                        lam_free_term(s);
                        lam_free((void*)x.s);
                        lam_free((void*)fresh_name.s);
                        return 0x0;
                    };
					body->abs.vname = fresh_name;
                }

                Lterm* b = lam_subst_inplace(body->abs.body, lam_lstr_dup(x), lam_clone(s));
                if (lam_invalid_term(b)) {
                    puts("DEBUG: invalid term"); 
                    lam_free(s);
                    lam_free(body);
                    lam_free((void*)x.s);
                    return b;
                }
                body->abs.body = b;
                lam_free((void*)x.s);
                lam_free_term(s);
                return body;
            } else { //x is captured by \x
                lam_free((void*)x.s);
                lam_free_term(s);
                return body;
            }
        }
        case Lapptag: {
            Lterm* fred = lam_subst_inplace(body->app.fun, lam_lstr_dup(x), lam_clone(s));
            if (lam_invalid_term(fred)) {
                lam_free((void*)x.s);
                lam_free_term(s);
                lam_free_term(body);
                return fred;
            }
            body->app.fun = fred;
            Lterm* pred = lam_subst_inplace(body->app.param, x, s);
            if (lam_invalid_term(pred)) {
                lam_free((void*)x.s);
                //TODO: free term?
                lam_free_term(body);
                return pred;
            }
            body->app.param = pred;
            return body;
        }
        default: {
            lam_free_term(body);
            lam_free_term(s);
            lam_free((void*)x.s);
             return lam_internal_error();
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
    if (!t) {
        lam_free_term(t);
        return lam_internal_error();
    }
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
                    Lterm* red = lam_subst_inplace(t->app.fun->abs.body, t->app.fun->abs.vname, t->app.param);
                    lam_free(t->app.fun);
                    lam_free(t);
                    return red;
                }
                return t;
            } else if (lam_normal_form(t->app.fun)) {
                Lterm* p = lam_reduce_step(t->app.param);
                if (lam_invalid_term(p)) {
                    lam_free_term(t);
                    return p;
                }
                t->app.param = p;
                return t;
            } else {
                Lterm* f = lam_reduce_step(t->app.fun);
                if (lam_invalid_term(f)) {
                    lam_free_term(t);
                    return f;
                }
                t->app.fun = f;
                return t;
            }
        }
        default: {
            lam_free_term(t);
            return lam_internal_error();
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
    if (lam_invalid_term(t)) { return t; }
    unsigned max_reductions = 1000;
    unsigned nreds = 0;

    unsigned tlen = UINT_MAX;
    unsigned theight = UINT_MAX;
    unsigned measure_freq = 37;

    for (
        ; nreds < max_reductions && !lam_normal_form(t)
        ; ++nreds, t = lam_reduce_step(t)
    ) {
        //if (nreds % 999 == 0) { lam_term_log(t, nreds); }
        if (nreds % measure_freq == 0) {
            unsigned new_tlen = lam_term_len(t);
            unsigned new_theight = lam_term_height(t);
            if (new_tlen >= tlen && new_theight >= theight) {
                lam_free_term(t);
                return lam_not_reducing();
            }
            tlen = new_tlen;
            theight = new_theight;
        }
    }
    if (nreds == max_reductions) {
        return lam_too_many_reductions();
    }
    return t;
}

