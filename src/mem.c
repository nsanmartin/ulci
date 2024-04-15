#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <gc/gc.h>
#include <stdlib.h>
#include <string.h>

#define MEM_SZ 1000000
uintptr_t allocated[MEM_SZ] = {0};
uintptr_t deallocated[MEM_SZ] = {0};
size_t last_alloc = 0;
size_t last_dealloc = 0;

void print_mem_summary() {
    puts("Allocated:");
    for (size_t i = 0; i < last_alloc; ++i) { printf("  %lx\n", allocated[i]); }
    puts("Deallocated:");
    for (size_t i = 0; i < last_dealloc; ++i) { printf("  %lx\n", deallocated[i]); }
}

void* lam_malloc(size_t size) {
#ifdef MEM_TEST
    void* r = malloc(size);
    assert(last_alloc < MEM_SZ);
    allocated[last_alloc++] = (uintptr_t)r;
    return r;
#else
    return malloc(size);
#endif
}

void* lam_calloc(size_t nmemb, size_t size) {
#ifdef MEM_TEST
    void* r = calloc(nmemb, size);
    assert(last_alloc < MEM_SZ);
    allocated[last_alloc++] = (uintptr_t)r;
    return r;
#else
    return calloc(nmemb, size);
#endif
}

void lam_free_error() { puts("error freind ptr "); }

void lam_free(void* ptr) {
#ifdef MEM_TEST
    assert(last_dealloc < MEM_SZ);
    uintptr_t to_free = (uintptr_t)ptr;
    deallocated[last_dealloc++] = to_free;
    for (size_t i = 0; i < last_alloc; ++i) {
        if (to_free == allocated[i]) { return; }
    }
    lam_free_error();
#else
    free(ptr);
#endif
}

char* lam_strdup(const char* s) {
#ifdef MEM_TEST
    assert(last_alloc < MEM_SZ);
    char* r = strdup(s);
    allocated[last_alloc++] = (uintptr_t)r;
    return r;
#else
    return strdup(s);
#endif
}

char* lam_strndup(const char* s, size_t n) {
#ifdef MEM_TEST
    assert(last_alloc < MEM_SZ);
    char* r = strndup(s,n);
    allocated[last_alloc++] = (uintptr_t)r;
    return r;
#else
    return strndup(s,n);
#endif
}

void lam_free_mem(void) { }

