#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <gc/gc.h>
#include <stdlib.h>
#include <string.h>

#define MEM_SZ 200
void* lam_memory[MEM_SZ] = {0};
int lam_free_mem_ix = 0;

#define MAX_ALLOC 1000000
uintptr_t allocated[MAX_ALLOC] = {0};
uintptr_t deallocated[MAX_ALLOC] = {0};
size_t last_alloc = 0;
size_t last_dealloc = 0;

void print_mem_summary() {
    puts("Allocated:");
    for (size_t i = 0; i < last_alloc; ++i) { printf("  %lx\n", allocated[i]); }
    puts("Deallocated:");
    for (size_t i = 0; i < last_dealloc; ++i) { printf("  %lx\n", deallocated[i]); }
}

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
    void* r = malloc(size);
    assert(last_alloc < MAX_ALLOC);
    allocated[last_alloc++] = (uintptr_t)r;
    return r;
#endif
}

void* lam_calloc(size_t nmemb, size_t size) {
    void* r = calloc(nmemb, size);
    assert(last_alloc < MAX_ALLOC);
    allocated[last_alloc++] = (uintptr_t)r;
    return r;
}

void lam_free_error() {
    puts("error freind ptr ");
}

void lam_free(void* ptr) {
    assert(last_dealloc < MAX_ALLOC);
    uintptr_t to_free = (uintptr_t)ptr;
    deallocated[last_dealloc++] = to_free;
    free(ptr);
    for (size_t i = 0; i < last_alloc; ++i) {
        if (to_free == allocated[i]) { return; }
    }
    lam_free_error();
}

char* lam_strdup(const char* s) {
    assert(last_alloc < MAX_ALLOC);
    char* r = strdup(s);
    allocated[last_alloc++] = (uintptr_t)r;
    return r;
}

void lam_free_mem(void) {
#ifdef TESTMEM
    printf("malloc calls: %d\n", lam_free_mem_ix);
    while(--lam_free_mem_ix >= 0) {
        free(lam_memory[lam_free_mem_ix]);
    }
#endif
}

char* lam_strndup(const char* s, size_t n) {
    assert(last_alloc < MAX_ALLOC);
    char* r = strndup(s,n);
    allocated[last_alloc++] = (uintptr_t)r;
    return r;
}

