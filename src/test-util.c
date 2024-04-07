#include <eval.h>
#include <recursive-descent.h>
#include <mem.h>
#include <str.h>

LtermList* stmts_str_to_evaluated_lst(const char* s) {
    ToListCallbackAcum acum = {0};
    StmtReadCallback callback[2] = {
        { .callback=eval_to_list, .acum=&acum },
        {0}
    };
    lam_scan_set_str_input(s);
    lam_parse_stmts(callback);
    return acum.first;
}

LstrList* newLstrListOrFail() {
    LstrList* rv = lam_malloc(sizeof(LstrList));
    if (!rv) { puts("Mem error"); exit(EXIT_FAILURE); }
    *rv = (LstrList){0};
    return rv;
}

LstrList* map_term_str(LtermList* ts) {
    if (!ts) { return NULL; }

    LstrList* rv = newLstrListOrFail();

    for (LstrList* it = rv;;) {
        it->s = lam_term_to_str_less_paren(ts->t);
        if (ts->next) {
            it->next = newLstrListOrFail();
            ts = ts->next;
            it = it->next;
        } else {
            break;
        }
    }
    return rv;
}
