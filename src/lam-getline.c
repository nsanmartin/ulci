#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <mem.h>


char* lam_readline (const char *prompt) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    printf("%s", prompt);
    read = getline(&line, &len, stdin);
    if (read == -1) { lam_free(line); line = 0x0; }
    return line;

}
