#include <stdio.h>
#include "parser-util.h"

void yyerror(const char* s) {
    fprintf(stderr, "error: %s\n", s);
}


/* Declarations */
void set_input_string(const char* in);
void end_lexical_scan(void);

/* This function parses a string */
Lstr parse_string(const char* in) {
  set_input_string(in);
  yyparse();
  end_lexical_scan();
  Lstr rv = lam_term_to_str(get_lam_term());
  return rv;
}

Lterm* _lterm;
void set_last_lam_term(Lterm* t) {
    lam_print_term(t);
    puts("");
    _lterm = t;
}

Lterm* get_lam_term(void) {
    return _lterm;
}
