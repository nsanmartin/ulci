#define _GNU_SOURCE
//#include <gc/gc.h>
#include <string.h>
#include <stdbool.h>

#include "lam.h"
#include "mem.h"


//// Lterm values that denote errors:
///
const Lterm* LamInternalError = &(Lterm){
    .var={.name={.s="LamInternalError"}}
};
// parse error
const Lterm* NotParse = &(Lterm){.var={.name={.s="NotParse"}}};
const Lterm* SyntaxError = &(Lterm){.var={.name={.s="SyntaxError"}}};
// eval errors
const Lterm* NotReducing = &(Lterm){.var={.name={.s="NotReducing"}}};
const Lterm* EvalStackTooLarge = &(Lterm){
    .var={.name={.s="EvalStackTooLarge"}}
};
////


long used_fresh_vars = 0;

Lstr lam_get_form_name(const Lterm t[static 1]) {
    switch(t->tag) {
        case Lvartag: return lam_str("Variable");
        case Labstag: return lam_str("Abstraction");
        case Lapptag: return lam_str("Application");
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}


const char* lam_get_form_name_cstr(const Lterm t[static 1]) {
    return lam_get_form_name(t).s;
}


/**
 * Factory methods
 **/


Lterm* lam_new_var(Lstr n) {
    if (lam_str_null(n)) { return 0x0; } 
    Lterm* rv = lam_malloc(sizeof (*rv));
    if (!rv) { return 0x0; }
    *rv = (Lterm) { .tag=Lvartag, . var = (Lvar) { .name = n }};
    return rv;
}

Lterm* lam_new_abs(Lstr vn, const Lterm body[static 1]) {
    if (lam_str_null(vn)) { return 0x0; } 
    Lterm* b = lam_clone(body);
    if (!b) {  return 0x0; }
    Lterm* rv = lam_malloc(sizeof (*rv));
    if (!rv) {  return 0x0; }
    *rv = (Lterm) { .tag = Labstag, .abs= (Labs) {.vname=vn, .body=b}};
    return rv;
}

Lterm* lam_new_app(const Lterm fun[static 1], const Lterm param[static 1]) {
    Lterm* f = lam_clone(fun);
    if (!f) { return 0x0; }
    Lterm* p = lam_clone(param);
    if (!p) {  return 0x0; }
    Lterm* rv = lam_malloc(sizeof(*rv));
    if (!rv) {  return 0x0; }
    *rv = (Lterm) {.tag=Lapptag, .app=(Lapp){.fun=f, .param=p}};
    return rv;
}



bool lam_is_var_free_in(const Lterm t[static 1], Lstr n) {

    switch(t->tag) {
        case Lvartag: { return lam_str_eq(n, t->var.name);}
        case Labstag: {
            return !lam_str_eq(n, t->abs.vname)
                && lam_is_var_free_in(t->abs.body, n);
        }
        case Lapptag: {
            return lam_is_var_free_in(t->app.fun, n)
                || lam_is_var_free_in(t->app.param, n);
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }

}


const char var_reserved_char = '#';

int count_trailing_left(Lstr s, const char c) {
    const char* p = s.s;
    while (*p == c) { ++p; }
    return p - s.s;
}


int lam_max_reserved_var_len(const Lterm t[static 1]) {
    switch(t->tag) {
        case Lvartag: {
            return count_trailing_left(t->var.name, var_reserved_char);
        }
        case Labstag: {
            //TODO: use S \ x if same?
            return lam_max_reserved_var_len(t->abs.body);
        }
        case Lapptag: {
            int fun_count = lam_max_reserved_var_len(t->app.fun); 
            int param_count = lam_max_reserved_var_len(t->app.param); 
            return fun_count > param_count ? fun_count : param_count;
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}




Lstr lam_get_fresh_var_name(const Lterm t[static 1]) {
    const int len = lam_max_reserved_var_len(t) + 1;
    char* rv = lam_malloc(sizeof(*rv) * (len+ 1));
    if (!rv) { return lam_str(0x0); }
    memset(rv, var_reserved_char, len);
    rv[len] = '\0';
    return lam_str(rv);
}

/**
 * Rename
 */



int lam_rename_var(Lterm t[static 1], Lstr varname, Lstr newname) {
    switch(t->tag) {
        case Lvartag: {
            if (lam_str_eq(varname, t->var.name)) {
                t->var.name = newname;
                if (lam_str_null(t->var.name)) { return -1; }

            }
            return 0;
        }
        case Labstag: {
            if (lam_str_eq(t->abs.vname, varname)) {
                t->abs.vname = newname;
                if (lam_str_null(t->abs.vname)) { return -1; }
            }
            return lam_rename_var(t->abs.body, varname, newname);
        }
        case Lapptag: {
            return lam_rename_var(t->app.fun, varname, newname) 
                + lam_rename_var(t->app.param, varname, newname);
                      }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}



Lterm* lam_clone(const Lterm t[static 1]) {
    switch(t->tag) {
        case Lvartag: {return lam_new_var(t->var.name);}
        case Labstag: {return lam_new_abs(t->abs.vname, t->abs.body);}
        case Lapptag: {return lam_new_app(t->app.fun, t->app.param);}
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}

/**
 * Substitute
 */


Lterm*
lam_substitute(const Lterm t[static 1], Lstr x, const Lterm s[static 1]) {
    switch(t->tag) {
        case Lvartag: {
            if (lam_str_eq(t->var.name, x)) {
                return lam_clone(s);
            } else {
                return lam_clone(t);
            }
        }
        case Labstag: {
            if (lam_is_var_free_in(t,x)) {

                Lterm* u = (Lterm*)t;
                if (lam_is_var_free_in(s, t->abs.vname)) {
                    u = lam_clone(u);
                    if (!u) { return 0x0; }
                    Lstr fresh_name = lam_get_fresh_var_name(u);
                    if (lam_str_null(fresh_name)) { return 0x0; }
                    lam_rename_var(u->abs.body, u->abs.vname, fresh_name);
					u->abs.vname = fresh_name;
                }

                Lterm* subst = lam_substitute(u->abs.body, x, s);
                if (!subst) { return 0x0; }
                Lstr vn = u->abs.vname;
                if (lam_str_null(vn)) { return 0x0; }
                Lterm* rv = lam_malloc(sizeof (*rv));
                if (!rv) { return 0x0; }
                *rv = (Lterm) {
                    .tag=Labstag,
                    .abs=(Labs) {.vname=vn, .body=subst}
                };
                return rv;
            } else { //x is captured by \x
                return lam_clone(t);
            }
        }
        case Lapptag: {
            Lterm* f_ = lam_substitute(t->app.fun, x, s);
            if (!f_) { return 0x0; }
            Lterm* p_ = lam_substitute(t->app.param, x, s);
            if (!p_) {  return 0x0; }
            Lterm* rv = lam_malloc(sizeof(*rv));
            if (!rv) {  return 0x0; }
            *rv = (Lterm) {
                .tag=Lapptag,
                .app = (Lapp){ .fun=f_, .param=p_ }
            };
            return rv;
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}



bool lam_are_identical(const Lterm t[static 1], const Lterm u[static 1]) {
    if (t->tag != u->tag) return false;

    switch(t->tag) {
        case Lvartag: {
            return lam_str_eq(t->var.name, u->var.name);
        }
        case Labstag: {
            return lam_str_eq(t->abs.vname, u->abs.vname) 
                && lam_are_identical(t->abs.body, u->abs.body);
        }
        case Lapptag: {
            return lam_are_identical(t->app.fun, u->app.fun)
                && lam_are_identical(t->app.param, u->app.param);
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}


void lam_print_term(const Lterm t[static 1]) {
    switch(t->tag) {
        case Lvartag: {
            printf("%s", lam_str_to_cstr(t->var.name));
            break;
        }
        case Labstag: {
            printf("(\\%s.", lam_str_to_cstr(t->abs.vname));
            lam_print_term(t->abs.body);
            printf(")");
            break;
        }
        case Lapptag: {
            printf("(");
            lam_print_term(t->app.fun);
            printf(" ");
            lam_print_term(t->app.param);
            printf(")");
            break;
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}

void lam_print_term_less_paren(const Lterm t[static 1]) {
    switch(t->tag) {
        case Lvartag: {
            lam_str_fwrite(t->var.name);
            break;
        }
        case Labstag: {
            fwrite("\\", 1, 1, stdout);
            lam_str_fwrite(t->abs.vname);
            fwrite(".", 1, 1, stdout);
            lam_print_term_less_paren(t->abs.body);
            break;
        }
        case Lapptag: {
            lam_print_term_less_paren(t->app.fun);
            fwrite(" ", 1, 1, stdout);
            if (t->app.param->tag == Lapptag || t->app.param->tag == Labstag) {
                fwrite("(", 1, 1, stdout);
                lam_print_term_less_paren(t->app.param);
                fwrite(")", 1, 1, stdout);
            } else {
                lam_print_term_less_paren(t->app.param);
            }
            break;
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}


Lstr lam_term_to_str_more_paren(const Lterm t[static 1]) {
    switch(t->tag) {
        case Lvartag: {
            return t->var.name;
        }
        case Labstag: {
            Lstr bstr = lam_term_to_str_more_paren(t->abs.body);
            if (!bstr.s) {
                perror("malloc returned null.");
                return LEMPTY_STR();
            }
            size_t len = lam_strlen(bstr);
            size_t lenrv = 1 + len + 4 + lam_strlen(t->abs.vname);
            char* buf = lam_malloc(sizeof(char) * lenrv);;
            if (!buf) {
                perror("malloc returned null.");
                return LEMPTY_STR();
            }
            size_t n = snprintf(buf, lenrv, "(\\%s.%s)", t->abs.vname.s, bstr.s);
            if (n >= lenrv) {
                perror("snprintf trucated string.");
                return LEMPTY_STR();
            }
            return lam_str(buf);
        }
        case Lapptag: {
            Lstr fstr = lam_term_to_str_more_paren(t->app.fun);
            if (!fstr.s) {
                perror("malloc returned null.");
                return LEMPTY_STR();
            }
            Lstr pstr = lam_term_to_str_more_paren(t->app.param);
            if (!pstr.s) {
                perror("malloc returned null.");
                return LEMPTY_STR();
            }

            size_t lenrv = 1 + lam_strlen(fstr) + lam_strlen(pstr) + 3;
            char* buf = lam_malloc(sizeof(char) * lenrv);
            if (!buf) { return LEMPTY_STR(); }
            size_t n = snprintf(buf, lenrv, "(%s %s)", fstr.s, pstr.s);
            if (n >= lenrv) {
                perror("snprintf trucated string.");
                return LEMPTY_STR();
            }
            return lam_str(buf);
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}

Lstr lam_term_to_str_less_paren(const Lterm t[static 1]) {
    switch(t->tag) {
        case Lvartag: {
            return t->var.name;
        }
        case Labstag: {
            Lstr bstr = lam_term_to_str_less_paren(t->abs.body);
            if (!bstr.s) {
                perror("malloc returned null.");
                return LEMPTY_STR();
            }
            size_t len = lam_strlen(bstr);
            size_t lenrv = 1 + len + 2 + lam_strlen(t->abs.vname);
            char* buf = lam_malloc(sizeof(char) * lenrv);;
            if (!buf) {
                perror("malloc returned null.");
                return LEMPTY_STR();
            }
            size_t n = snprintf(buf, lenrv, "\\%s.%s", t->abs.vname.s, bstr.s);
            if (n >= lenrv) {
                perror("snprintf trucated string.");
                return LEMPTY_STR();
            }
            return lam_str(buf);
        }
        case Lapptag: {
            Lstr fstr = lam_term_to_str_less_paren(t->app.fun);
            if (!fstr.s) {
                perror("malloc returned null.");
                return LEMPTY_STR();
            }
            Lstr pstr = lam_term_to_str_less_paren(t->app.param);
            if (!pstr.s) {
                perror("malloc returned null.");
                return LEMPTY_STR();
            }

            size_t nparen = 0;
            const char* fmt = "%s %s";

            if (t->app.fun->tag != Labstag && t->app.param->tag == Lvartag) {
            } else if (t->app.fun->tag == Labstag && t->app.param->tag == Lapptag) {
                nparen = 4;
                fmt = "(%s) (%s)";
            } else if ((t->app.fun->tag == Lvartag && t->app.param->tag != Lvartag)
                    || (t->app.fun->tag == Lapptag && t->app.param->tag != Lvartag)){
                nparen = 2;
                fmt = "%s (%s)";
            } else if ((t->app.fun->tag == Labstag && t->app.param->tag == Labstag)) {
                nparen = 2;
                fmt = "(%s) %s";
            }

            size_t lenrv = 1 + lam_strlen(fstr) + lam_strlen(pstr) + 1 + nparen;
            char* buf = lam_malloc(sizeof(char) * lenrv);
            if (!buf) { return LEMPTY_STR(); }
            size_t n = snprintf(buf, lenrv, fmt, fstr.s, pstr.s);
            if (n >= lenrv) {
                perror("snprintf trucated string.");
                return LEMPTY_STR();
            }
            return lam_str(buf);
            
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}

bool lam_normal_form(const Lterm t[static 1]) {
    return t->tag == Lvartag
        || (t->tag == Labstag && lam_normal_form(t->abs.body))
        || (t->tag == Lapptag
                && (t->app.fun->tag != Labstag)
                && lam_normal_form(t->app.fun)
                && lam_normal_form(t->app.param))
        ;
}
