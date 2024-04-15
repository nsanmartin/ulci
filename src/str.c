#include <lam.h>

void freeLstrList(LstrList* ls) {
    while (ls) {
        LstrList* tmp = ls;
        ls = ls->next;
        lam_free((void*)tmp->s.s);
        lam_free(tmp);
    }
}

