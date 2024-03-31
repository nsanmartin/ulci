#include <stdio.h>
#include <gc/gc.h>
#include <stdlib.h>

#define MEM_SZ 200
void* lam_memory[MEM_SZ] = {0};
int lam_free_mem_ix = 0;


void* lam_malloc(size_t size) {
#ifdef TESTMEM
    if (lam_free_mem_ix < MEM_SZ) {
        void* rv = malloc(size);
        if (!rv) {
            perror("lam_malloc: not enough memory");
            abort();
        }
        ++lam_free_mem_ix;
        return rv;
    } else {
        perror("lam_malloc: MEM_SZ reached");
        abort();
    }
#else
    return GC_malloc(size);
#endif
}

void lam_free_mem(void) {
#ifdef TESTMEM
    printf("malloc calls: %d\n", lam_free_mem_ix);
    while(--lam_free_mem_ix >= 0) {
        free(lam_memory[lam_free_mem_ix]);
    }
#endif
}

char* lam_strdup(const char* s) {
    //TODO: TESTMEM?
    return GC_strdup(s);
}


