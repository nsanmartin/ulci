#include <parser-lam-reader.h>
#include <parser-names.h>

int main (void) {
    parser_set_repl_fn();
    if(initialize_symbol_table()) {
        fprintf(stderr, "Erroe: not memory to initialize parser\n");
        return -1;
    }
    yyin = stdin;
    yyparse();
}
