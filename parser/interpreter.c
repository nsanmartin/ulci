#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <lexer.h>
#include <eval.h>
#include <symbols.h>
#include <recursive-descent.h>
#include <reduce.h>

void free_term_callback(Lterm t[static 1], void* ignore) {
    (void) ignore;
    lam_free_term(t);
}

void reduce_print(Lterm t[static 1], void* ignore) {
    (void) ignore;
    void (*on_parse)(const Lterm t[static 1]) = lam_print_term_less_paren;

    t = lam_reduce(t);
    if (t == NotReducing) {
        printf("eval error: %s\nterm: '", "term is not reducing");
        on_parse(t);
        puts("'");
    } else if (t == EvalStackTooLarge) {
        printf("eval error: %s\nterm: '", "eval stack too large");
        on_parse(t);
        puts("'");
    } else if (!t || t == LamInternalError) {
        puts("Lam eval internal error");
    } else {
        on_parse(t);
    }
    puts("");
}


int interactive_interpreter(StmtReadCallback* callback) {
    char* line = NULL;
    while ((line = readline("> "))) {
        if (line && *line) {
            lam_scan_set_str_input(line);
            lam_parse_stmts(callback);
            add_history(line);
        }
    }
    return 0;
}


int main (int argc, char* argv[]) {
    StmtReadCallback callback[3] = {
        { .callback=reduce_print },
        { .callback=free_term_callback },
        {0}
    };
    if(initialize_symbol_table()) {
        fprintf(stderr, "Error: not memory to initialize parser\n");
        return -1;
    }
    if (argc == 1) {
        interactive_interpreter(callback);
    } else {
        for (int i = 1; i < argc; ++i) {
            FILE* fp = fopen(argv[i], "r");
            if (fp) {
                lam_scan_set_file_input(fp);
                lam_parse_stmts(callback);
            } else {
                printf("Could not read %s\n", argv[i]);
            }
        }
    }
    return 0;
}
