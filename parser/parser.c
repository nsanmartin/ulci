#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <parser-lam-reader.h>
#include <parser-names.h>


int interactive() {
    char* line = NULL;
    while ((line = readline("> "))) {;
        parse_string(line, lam_term_to_str_less_paren);
        if (line && *line) {
            add_history(line);
        }
    }
    return 0;
}


int main (void) {
    parser_set_repl_fn();
    if(initialize_symbol_table()) {
        fprintf(stderr, "Erroe: not memory to initialize parser\n");
        return -1;
    }

    if (isatty(fileno(stdin))) {
        interactive();
    } else {
        yyin = stdin;
        yyparse();
    }
}
