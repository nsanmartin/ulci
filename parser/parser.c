#include <parser-lam-reader.h>

int main (void) {
    printf("> ");
    yyin = stdin;
    yyparse();
}
