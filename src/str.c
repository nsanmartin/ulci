#include <lam.h>

void freeLstrList(LstrList* ls) {
    while (ls) {
        LstrList* tmp = ls;
        ls = ls->next;
        free((void*)tmp->s.s);
        free(tmp);
    }
}

