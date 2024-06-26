#define _GNU_SOURCE
#include <string.h>
#include <stdbool.h>

#include "lam.h"
#include "mem.h"
#include <reduce.h>
#include <recursive-descent.h>


void lam_free_term(Lterm* t) {
    if (!t) { puts("DEBUG: freeing NULL term :/"); LOG_INVALID_LTERM_AND_EXIT; }

    switch (t->tag) {
        case Lvartag: {
            lam_free((void*)t->var.name.s);
            lam_free(t);
            return;
        }
        case Labstag: {
            lam_free((void*)t->abs.vname.s);
            lam_free_term(t->abs.body);
            lam_free(t);
            return;
        }
        case Lapptag: {
            lam_free_term(t->app.fun);
            lam_free_term(t->app.param);
            lam_free(t);
            return;
        }
        case Lerrtag: {
            // not needed since we are using literal strings
            //if (t->err.msg) {
            //    lam_free((char*)t->err.msg);
            //}
            lam_free(t);
            return;
        }
        default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}

void freeLtermList(LtermList* ls) {
    while (ls) {
        LtermList* tmp = ls;
        ls = ls->next;
        lam_free_term((Lterm*)tmp->t);
        lam_free((void*)tmp);
    }
}


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

/**
 * Factory methods
 **/

Lterm* lam_not_parse(const char* msg, RecDescCtx* ctx) {
    Lterm* rv = lam_malloc(sizeof (Lterm));
    if (!rv) { return 0x0; }
    *rv = (Lterm) {
        .tag=Lerrtag,
        .err = (Lerr) {
            .tag=LNotParseTag,
            .msg=msg,
            .tk=ctx->last,
            .col=ctx->buf.col
        }
    };
    return rv;
}

Lterm* lam_syntax_error(const char* msg, RecDescCtx* ctx) { //LamTokenTag tk, size_t col) {
    Lterm* rv = lam_malloc(sizeof (Lterm));
    if (!rv) { return 0x0; }
    *rv = (Lterm) {
        .tag=Lerrtag,
        .err = (Lerr) {
            .tag=LSyntaxErrorTag,
            .msg=msg,
            .tk=ctx->last,
            .col=ctx->buf.col
        }
    };
    return rv;
}

Lterm* lam_internal_error(void) {
    Lterm* rv = lam_malloc(sizeof (Lterm));
    if (!rv) { return 0x0; }
    *rv = (Lterm) { .tag=Lerrtag, .err = (Lerr) { .tag=LInternalErrorTag, .msg=0x0 }};
    return rv;
}

Lterm* lam_not_reducing(void) {
    Lterm* rv = lam_malloc(sizeof (Lterm));
    if (!rv) { return 0x0; }
    *rv = (Lterm) { .tag=Lerrtag, .err = (Lerr) { .tag=LNotReducingTag, .msg=0x0 }};
    return rv;
}

Lterm* lam_too_many_reductions(void) {
    Lterm* rv = lam_malloc(sizeof (Lterm));
    if (!rv) { return 0x0; }
    *rv = (Lterm) { .tag=Lerrtag, .err = (Lerr) { .tag=LTooManyReductionsTag, .msg=0x0 }};
    return rv;
}

Lterm* lam_lexical_error(const char* msg) {
    Lterm* rv = lam_malloc(sizeof (Lterm));
    if (!rv) { return 0x0; }
    *rv = (Lterm) { .tag=Lerrtag, .err = (Lerr) { .tag=LLexicalError, .msg=msg }};
    return rv;
}


Lterm* lam_new_var(Lstr n) {
    if (lam_str_null(n)) { return 0x0; } 
    Lstr m = lam_lstr_dup(n);
    if (lam_str_null(m)) {
        return 0x0;
    }
    Lterm* rv = lam_malloc(sizeof (*rv));
    if (!rv) {
        lam_free((void*)m.s);
        return 0x0;
    }
    *rv = (Lterm) { .tag=Lvartag, . var = (Lvar) { .name = m }};
    return rv;
}

Lterm* lam_var(Lstr n) {
    if (lam_str_null(n)) {
        return 0x0;
    } 
    Lterm* rv = lam_malloc(sizeof (*rv));
    if (!rv) {
        lam_free((void*)n.s);
        return 0x0;
    }
    *rv = (Lterm) { .tag=Lvartag, . var = (Lvar) { .name = n }};
    return rv;
}

Lterm* lam_new_abs(Lstr vn, const Lterm body[static 1]) {
    if (lam_str_null(vn)) { return 0x0; } 
    Lterm* b = lam_clone(body);
    if (!b) {  return 0x0; }
    Lstr vname = lam_lstr_dup(vn);
    if (lam_str_null(vname)) {
        lam_free_term(b);
        return 0x0;
    }
    Lterm* rv = lam_malloc(sizeof (*rv));
    if (!rv) {
        lam_free_term(b);
        lam_free((void*)vname.s);
        return 0x0;
    }
    *rv = (Lterm) { .tag = Labstag, .abs= (Labs) {.vname=vname, .body=b}};
    return rv;
}

Lterm* lam_abs(Lstr vn, Lterm body[static 1]) {
    if (lam_str_null(vn)) {
        lam_free_term(body);
        return 0x0;
    } 
    Lterm* rv = lam_malloc(sizeof (*rv));
    if (!rv) {
        lam_free_term(body);
        lam_free((void*)vn.s);
        return 0x0;
    }
    *rv = (Lterm) { .tag = Labstag, .abs= (Labs) {.vname=vn, .body=body}};
    return rv;
}


Lterm* lam_new_app(const Lterm fun[static 1], const Lterm param[static 1]) {
    Lterm* f = lam_clone(fun);
    if (!f) { return 0x0; }
    Lterm* p = lam_clone(param);
    if (!p) {
        lam_free_term(f);
        return 0x0;
    }
    Lterm* rv = lam_malloc(sizeof(*rv));
    if (!rv) {
        lam_free_term(f);
        lam_free_term(p);
        return 0x0;
    }
    *rv = (Lterm) {.tag=Lapptag, .app=(Lapp){.fun=f, .param=p}};
    return rv;
}

Lterm* lam_app(Lterm fun[static 1], Lterm param[static 1]) {
    Lterm* rv = lam_malloc(sizeof(*rv));
    if (!rv) {
        lam_free_term(fun);
        lam_free_term(param);
        return 0x0;
    }
    *rv = (Lterm) {.tag=Lapptag, .app=(Lapp){.fun=fun, .param=param}};
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
    char* rv = lam_malloc(sizeof(char) * (len + 1));
    if (!rv) {
        puts("Not memory, aborting."); exit(EXIT_FAILURE);
    }
    memset(rv, var_reserved_char, len);
    rv[len] = '\0';
    return lam_strn_view(rv, len);
}

/**
 * Rename
 */



int lam_rename_var(Lterm t[static 1], Lstr varname, Lstr newname) {
    //TODO: review
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


int lam_rename_var_in_place(Lterm t[static 1], Lstr varname, Lstr newname) {
    switch(t->tag) {
        case Lvartag: {
            if (lam_str_eq(varname, t->var.name)) {
                lam_free((void*)t->var.name.s);
                t->var.name = lam_lstr_dup(newname);
                if (lam_str_null(t->var.name)) {
                    return -1;
                }
            }
            return 0;
        }
        case Labstag: {
            if (lam_str_eq(t->abs.vname, varname)) {
                lam_free((void*)t->abs.vname.s);
                t->abs.vname = lam_lstr_dup(newname);
                if (lam_str_null(t->abs.vname)) {
                    return -1;
                }
            }
            return lam_rename_var_in_place(t->abs.body, varname, newname);
        }
        case Lapptag: {
            return lam_rename_var_in_place(t->app.fun, varname, newname) 
                 | lam_rename_var_in_place(t->app.param, varname, newname);
       }
       default: LOG_INVALID_LTERM_AND_EXIT ;
    }
}



Lterm* lam_clone(const Lterm t[static 1]) {
    if (lam_invalid_term(t)) { LOG_INVALID_LTERM_AND_EXIT ; }
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
lam_subst_dup(const Lterm t[static 1], Lstr x, const Lterm s[static 1]) {
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

                Lterm* subst = lam_subst_dup(u->abs.body, x, s);
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
            Lterm* f_ = lam_subst_dup(t->app.fun, x, s);
            if (!f_) { return 0x0; }
            Lterm* p_ = lam_subst_dup(t->app.param, x, s);
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
                perror("lam_malloc returned null.");
                return LEMPTY_STR();
            }
            size_t len = lam_strlen(bstr);
            size_t lenrv = 1 + len + 4 + lam_strlen(t->abs.vname);
            char* buf = lam_malloc(sizeof(char) * lenrv);;
            if (!buf) {
                perror("lam_malloc returned null.");
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
                perror("lam_malloc returned null.");
                return LEMPTY_STR();
            }
            Lstr pstr = lam_term_to_str_more_paren(t->app.param);
            if (!pstr.s) {
                perror("lam_malloc returned null.");
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
        case Lerrtag: {
            perror("parse failed");
            return LEMPTY_STR();
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
                perror("lam_malloc returned null.");
                return LEMPTY_STR();
            }
            size_t len = lam_strlen(bstr);
            size_t lenrv = 1 + len + 2 + lam_strlen(t->abs.vname);
            char* buf = lam_malloc(sizeof(char) * lenrv);;
            if (!buf) {
                perror("lam_malloc returned null.");
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
                perror("lam_malloc returned null.");
                return LEMPTY_STR();
            }
            Lstr pstr = lam_term_to_str_less_paren(t->app.param);
            if (!pstr.s) {
                perror("lam_malloc returned null.");
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
        case Lerrtag: {
            perror("parse failed");
            return LEMPTY_STR();
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

/*
 * Utils
 */

// Callbacks
// pcb (Parse CallBacks)

void lam_pcb_id(Lterm* tptr[static 1], /*(Lterm**)*/void* rvp) { *(Lterm**)rvp = *tptr; }

void free_term_callback(Lterm* tptr[static 1], void* ignore) {
    (void) ignore;
    lam_free_term(*tptr);
}

void lam_pcb_reduce(Lterm* tptr[static 1], /*Lterm** */void* rvp) {
    (void)rvp;
    *tptr = lam_reduce(*tptr);
}

void lam_pcb_print(Lterm* tptr[static 1], /*Lterm** */void* rvp) {
    (void)rvp;
    void (*print_fn)(const Lterm t[static 1]) = lam_print_term_less_paren;
    print_fn(*tptr);
    puts("");
}

void reduce_print_free_callback(Lterm* tptr[static 1], void* ignore) {
    (void) ignore;
    void (*on_parse)(const Lterm t[static 1]) = lam_print_term_less_paren;

    Lterm* t = lam_reduce(*tptr);
    *tptr = t;

    if (!t) { puts("Lam internal error: unexpected NULL term, aborting."); exit(EXIT_FAILURE); }
    if (t->tag == Lerrtag) {
        switch (t->err.tag) {
            case LNotParseTag: {
                const char* tk = lam_token_to_str(t->err.tk);
                printf("Parse failed at col %ld (tk: %s): %s", t->err.col, tk, t->err.msg);
                break;
            }
            case LLexicalError: {
                printf("Lexical error: %s",  t->err.msg);
                break;
            }
            case LSyntaxErrorTag: {
                const char* tk = lam_token_to_str(t->err.tk);
                printf("Syntax error at col %ld (tk: %s): %s", t->err.col, tk, t->err.msg);
                break;
            }
            case LNotReducingTag: {
                printf("eval error: term is not reducing");
                break;
            }
            case LTooManyReductionsTag: {
                printf("eval error: too many reductions");
                break;
            }
            case LInternalErrorTag:
            case LerrorTagCount: {
                printf("Lam internal error");
                break;
            }
        }
    } else {
        on_parse(t);
    }
    puts("");
    lam_free_term(t);
}


// Eval

Lterm* parse_string_rec_desc(const char* in) {
    Lterm* t = 0x0;
    StmtReadCallback callback[2] = { { .callback=lam_pcb_id, .acum=&t}, {0} };
    lam_scan_set_str_input(in);
    lam_parse_stmts(callback);
    return t;
}

Lstr parse_string_rec_desc_to_str(const char* in, Lstr (*to_str)(const Lterm[static 1])) {
    Lterm* t = parse_string_rec_desc(in);
    Lstr rv = to_str(t);
    return rv;
}

Lstr parse_string_rec_desc_reduce_to_str(const char* in, Lstr (*to_str)(const Lterm[static 1])) {
    Lterm* t = parse_string_rec_desc(in);
    Lstr rv = to_str(lam_reduce(t));
    return rv;
}

