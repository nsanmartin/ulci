#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <lexer.h>
#include <parser-lam-reader.h>
#include <symbols.h>

void scan_eol() {
    LamTokenTag tk = LamTokenTagsLen;
    for(; tk != LEof;) {
        LamKeywordBuf buf = {0};
        tk = lam_scan_next(&buf);
        const char* n = lam_token_to_str(tk);
        if (n) {
            printf("%s ", n);
            if (tk == LVar) {
                fwrite("[", 1, 1, stdout);
                fwrite(buf.s, 1, buf.len, stdout);
                fwrite("] ", 1, 2, stdout);
            }
        } else {
            puts("ERROR ><M");
            exit(1);
        }
    }
    puts("");
}

int interactive_lexer() {
    char* line = NULL;
    while ((line = readline("> "))) {
        if (line && *line) {
            lam_scan_set_str_input(line);
            scan_eol();
            add_history(line);
        }
    }
    return 0;
}


int main (int argc, char* argv[]) {
    if (argc == 1) {
        //printf("> ");
        interactive_lexer();
    } else {
        for (int i = 1; i < argc; ++i) {
            FILE* fp = fopen(argv[i], "r");
            if (fp) {
                lam_scan_set_file_input(fp);
                scan_eol();
            } else {
                printf("Could not read %s\n", argv[i]);
            }
        }
    }
    return 0;
}

