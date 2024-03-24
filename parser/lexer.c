#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <parser-lam-reader.h>
#include <parser-names.h>


//int interactive() {
//    char* line = NULL;
//    while ((line = readline("> "))) {;
//        parse_string(line, lam_term_to_str_less_paren);
//        if (line && *line) {
//            add_history(line);
//        }
//    }
//    return 0;
//}


int main (int argc, char* argv[]) {
    //yydebug = 1;

    //parser_set_repl_fn();
    if(initialize_symbol_table()) {
        fprintf(stderr, "Erroe: not memory to initialize parser\n");
        return -1;
    }

    if (argc == 1) {
        //interactive();
    } else {
        for (int i = 1; i < argc; ++i) {
            yyin = fopen(argv[i], "r");
            if (yyin) {
                yylex();
            }
        }
    }
}
