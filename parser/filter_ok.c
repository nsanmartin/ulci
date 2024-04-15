#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <lexer.h>
#include <eval.h>
#include <symbols.h>
#include <recursive-descent.h>
#include <reduce.h>

//FILE* stdoutf = stdout;
//FILE* stderrf = stderr;

void pcb_print(Lterm* tptr[static 1], /*Lterm** */void* rvp) {
    (void)rvp;
    Lterm* red = lam_reduce(lam_clone(*tptr));
    if (red && red->tag != Lerrtag) {
        lam_print_term_less_paren(*tptr);
        lam_free_term(red);
        lam_free_term(*tptr);
        puts("");
    }
}

int main (int argc, char* argv[]) {
    StmtReadCallback callback[3] = {
        { .callback=pcb_print },
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
