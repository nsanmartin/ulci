#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <lexer.h>
#include <eval.h>
#include <lam-symbols.h>
#include <recursive-descent.h>

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
    StmtReadCallback callback = { .callback=eval_print };
    if (argc == 1) {
        interactive_interpreter(&callback);
    } else {
        for (int i = 1; i < argc; ++i) {
            FILE* fp = fopen(argv[i], "r");
            if (fp) {
                lam_scan_set_file_input(fp);
                lam_parse_stmts(&callback);
            } else {
                printf("Could not read %s\n", argv[i]);
            }
        }
    }
    return 0;
}
