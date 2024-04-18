#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <parser-lam-reader.h>
#include <symbols.h>


int interactive(void) {
    char* line = NULL;
    while ((line = readline("> "))) {
        if (line && *line) {
            eval_string(line, lam_term_to_str_less_paren);
            add_history(line);
        }
    }
    return 0;
}


int main (int argc, char* argv[]) {
    //yydebug = 1;

    if(initialize_symbol_table()) {
        fprintf(stderr, "Erroe: not memory to initialize parser\n");
        return -1;
    }

    parser_set_repl_fn();
    if (argc == 1) {
        interactive();
    } else {
        for (int i = 1; i < argc; ++i) {
            yyin = fopen(argv[i], "r");
            if (yyin) {
                yyparse();
            }
        }
    }
}
