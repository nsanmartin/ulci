#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <lexer.h>
#include <eval.h>
#include <symbols.h>
#include <recursive-descent.h>
#include <reduce.h>


int main (int argc, char* argv[]) {
    StmtReadCallback callback[3] = {
        { .callback=reduce_print_free_callback },
        {0}
    };
    if(initialize_symbol_table()) {
        fprintf(stderr, "Error: not memory to initialize parser\n");
        return -1;
    }

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

    free_symbol_table();
    return 0;
}
