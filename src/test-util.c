#include <eval.h>
#include <recursive-descent.h>

LtermList* stmts_str_to_evaluated_str(const char* s) {
    LtermList* rv = lam_malloc(sizeof(LtermList));
    if (!rv) {
        puts("Memory error");
        exit(EXIT_FAILURE);
    }
    StmtReadCallback callback = { .callback=eval_to_list, .acum=rv };
    lam_scan_set_str_input(s);
    lam_parse_stmts(&callback);
    return rv;
}
