#include <parser-lam-reader.h>
#include <eval.h>

void yyerror(const char* s) {
    fprintf(stderr, "error: " RED "%s" RESET "\n", s);
}

Lterm* _last_lterm = 0x0;

int set_last_lam_term(Lterm t[static 1]) {
    lam_print_term(t);
    _last_lterm = t;
    return 0;
}

Lterm* get_last_lam_term(void) { return _last_lterm; }
void reset_last_lam_term(void) { _last_lterm = 0x0; }

#define LamStr(S) (Lstr){.s=S,.len=sizeof(S)-1}

Lstr parse_string(const char* in, Lstr (*to_str)(const Lterm[static 1])) {
    reset_last_lam_term();
    set_input_string(in);
    yyparse();
    end_lexical_scan();
    Lterm* last = get_last_lam_term();
    if (last) {
        Lstr rv = to_str(last);
        return rv;
    }
    return LamStr("Error: no parse");
}


Lstr eval_string(const char* in, Lstr (*to_str)(const Lterm[static 1])) {
    reset_last_lam_term();
    set_input_string(in);
    yyparse();
    end_lexical_scan();
    Lterm* last = get_last_lam_term();
    if (last) {
        Lstr rv = to_str(lam_eval(last));
        return rv;
    }
    return LamStr("Error: no parse");
}

void read_eval_print_promt(const Lterm t[static 1]) {
    Lterm* v = lam_eval(t);
    if (v) {
        lam_print_term_less_paren(v);
    } else {
        printf("No parse");
    }
    puts("");
}

//todo: choose the funtion with a cli param
void (*_lam_print_term_fn)(const Lterm t[static 1]) = 0x0;//lam_print_term;
//void (*_lam_print_term_fn)(const Lterm t[static 1]) = lam_print_term_less_paren;

void parser_set_repl_fn(void) {
    _lam_print_term_fn = read_eval_print_promt;
}
/*
 * This function is used by the parser each time an expresion is read.
 * See parser.y
 **/
int parser_read_expression(Lterm t[static 1]) {
    if (_lam_print_term_fn) {
        _lam_print_term_fn(t);
    };
    _last_lterm = t;
    return 0;
}

