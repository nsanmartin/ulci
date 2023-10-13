#include <parser-lam-reader.h>

int main (void) {
    parser_set_repl_fn();
    yyin = stdin;
    yyparse();
}
