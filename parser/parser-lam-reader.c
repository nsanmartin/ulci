#include <parser-lam-reader.h>

void yyerror(const char* s) { fprintf(stderr, "error: %s\n", s); }

Lterm* _last_lterm = 0x0;

int set_last_lam_term(Lterm t[static 1]) {
    lam_print_term(t);
    puts("");
    _last_lterm = t;
    return 0;
}

Lterm* get_last_lam_term(void) {
    //todo: if null panic
    return _last_lterm;
}

Lstr parse_string(const char* in) {
  set_input_string(in);
  yyparse();
  end_lexical_scan();
  Lstr rv = lam_term_to_str(get_last_lam_term());
  return rv;
}


void (*_lam_print_term_fn)(const Lterm t[static 1]) = lam_print_term_less_paren;

/* This is he function used by the parser each time an expresion is read. See parser.y */
int parser_read_expression(Lterm t[static 1]) {
    _lam_print_term_fn(t);
    _last_lterm = t;
    printf("\n> ");
    return 0;
}

