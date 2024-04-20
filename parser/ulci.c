#include <unistd.h>
#include <stdio.h>

#ifndef NO_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#else
#include <lam-readline.h>
#endif

#include <lexer.h>
#include <eval.h>
#include <symbols.h>
#include <recursive-descent.h>
#include <reduce.h>


int interactive_interpreter(StmtReadCallback* callback) {
    char* line = NULL;
    while ((line = readline("> "))) {
        if (line && *line) {
            lam_scan_set_str_input(line);
            lam_parse_stmts(callback);
            add_history(line);
        }
        lam_free(line);
    }
    return 0;
}


int main (int argc, char* argv[]) {
    StmtReadCallback callback[2] = { { .callback=reduce_print_free_callback }, {0} };
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
                fclose(fp);
            } else {
                printf("Could not read %s\n", argv[i]);
            }
        }
    }
    free_symbol_table();
    print_mem_summary();
    return 0;
}
