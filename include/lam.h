#ifndef __LAM_H_
#define __LAM_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mem.h>
#include <str.h>

#define LOG_INVALID_LTERM                                              \
        fprintf(                                                       \
            stderr,                                                    \
            "\033[91m"                                                 \
            "lam fatal error:\n================"                       \
            "\033[0m"                                                  \
            "\n\tInvalid term form.\n"                                 \
                "file: %s"                                             \
                ":%d\n"                                                \
                "func: %s\n",                                          \
                __FILE__,                                              \
                __LINE__,                                              \
                __func__)

#define LOG_INVALID_LTERM_AND_EXIT                                     \
    LOG_INVALID_LTERM; exit(EXIT_FAILURE)

typedef enum { Lvartag, Labstag, Lapptag } Lamtag;

typedef struct Lterm Lterm;

typedef struct { Lstr name; } Lvar;
typedef struct { Lstr vname; Lterm* body; } Labs;
typedef struct { Lterm* fun; Lterm* param; } Lapp;

typedef struct Lterm {
    Lamtag tag;
    union {
        Lvar var;
        Labs abs;
        Lapp app;
    }; //term;
} Lterm;

// Lterm errors
extern const Lterm* LamInternalError;
// parse errors
extern const Lterm* NotParse;
extern const Lterm* SyntaxError;
// eval errors
extern const Lterm* NotReducing;
extern const Lterm* EvalStackTooLarge;
////


typedef struct LtermList {
    const Lterm* t;
    struct LtermList* next;
} LtermList;


void freeLtermList(LtermList* ls);

static inline bool lam_invalid_term(const Lterm* t) {
    return !t
        || t == LamInternalError
        || t == NotParse
        || t == SyntaxError
        || t == NotReducing
        || t == EvalStackTooLarge;
}

static inline bool lam_eval_error(const Lterm* t) {
    return !t || t == LamInternalError || t == EvalStackTooLarge
        || t == NotReducing;
}

#define LVAR(NAME)                                                      \
    (Lterm){ .tag=Lvartag, .var=(Lvar){.name=NAME}} 

#define LABS(VNAME, BODY)                                               \
    (Lterm){.tag=Labstag, .abs=(Labs){.vname=VNAME, .body=&BODY}}

#define LAPP(FUN, PARAM)                                                \
    (Lterm) {.tag=Lapptag, .app=(Lapp){.fun=&FUN,.param=&PARAM}}

unsigned lam_term_len(const Lterm* t);
unsigned lam_term_height(const Lterm* t);

Lterm* lam_var(Lstr n);
Lterm* lam_abs(Lstr vn, Lterm body[static 1]);
Lterm* lam_app(Lterm fun[static 1], Lterm param[static 1]);

Lstr lam_get_form_name(const Lterm t[static 1]) ;
void lam_free_term(Lterm* t) ;
//static inline void lam_free_term_callback(void* t) {
//    lam_free_term((Lterm*) t);
//}

Lterm* lam_clone(const Lterm t[static 1]) ;
Lterm* lam_new_var(Lstr x) ;
Lterm* lam_new_abs(Lstr x, const Lterm body[static 1]) ;
Lterm* lam_new_app(const Lterm fun[static 1], const Lterm param[static 1]) ;

int lam_init_var(Lterm t[static 1], Lstr x) ;
int lam_init_abs(Lterm t[static 1], Lstr x, Lterm body[static 1]) ;
int
lam_init_app(Lterm t[static 1], Lterm fun[static 1], Lterm param[static 1]);
Lstr lam_term_to_str_more_paren(const Lterm t[static 1]) ;
Lstr lam_term_to_str_less_paren(const Lterm t[static 1]) ;
static inline Lstr lam_term_to_str(const Lterm t[static 1]) {
    return lam_term_to_str_more_paren(t);
}
bool lam_is_var_free_in(const Lterm t[static 1], Lstr n) ;
int lam_max_reserved_var_len(const Lterm t[static 1]) ;
Lstr lam_get_fresh_var_name(const Lterm t[static 1]) ;
int lam_rename_var(Lterm t[static 1], Lstr varname, Lstr newname) ;
const char* lam_get_form_name_cstr(const Lterm t[static 1]) ;
bool lam_are_identical(const Lterm t[static 1], const Lterm u[static 1]) ;
Lterm*
lam_substitute(const Lterm t[static 1], Lstr x, const Lterm s[static 1]) ;
void lam_print_term(const Lterm t[static 1]) ;
void lam_print_term_less_paren(const Lterm t[static 1]) ;

bool lam_normal_form(const Lterm t[static 1]) ;
inline static bool lam_valid_tag(const Lterm t[static 1]) {
    return t->tag == Lvartag || t->tag == Labstag || t->tag == Lapptag;
}

// Utils
void free_term_callback(Lterm* t[static 1], void* ignore);
void reduce_ret_callback(Lterm* t[static 1], /*Lterm*/void* rv);
void reduce_print_free_callback(Lterm* t[static 1], void* ignore);
Lstr parse_string_rec_desc_to_str(const char* in, Lstr (*to_str)(const Lterm[static 1]));
void free_term_callback(Lterm* tptr[static 1], void* ignore);
Lstr parse_string_rec_desc_reduce_to_str(const char* in, Lstr (*to_str)(const Lterm[static 1]));
#endif // __LAM_H_
