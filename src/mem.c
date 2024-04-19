#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <gc/gc.h>
#include <stdlib.h>
#include <string.h>

#define MEM_TEST

#define MEM_SZ 1000000
uintptr_t allocated[MEM_SZ] = {0};
uintptr_t deallocated[MEM_SZ] = {0};
size_t last_alloc = 0;
size_t last_dealloc = 0;

void check_all_freed() {
    for (size_t i = 0; i < last_alloc; ++i) {
        uintptr_t allocd = allocated[i];

        size_t j = 0; 
        for (; j < last_dealloc; ++j) {
            uintptr_t deallocd =  deallocated[j];
            if (allocd == deallocd) {
                printf("%lx ok\n", allocd);
                break;
            }
        }
        if (j == last_dealloc) {
            printf("%lx Not Freed!!\n", allocd);
        }
    }
}

void clear_allocated() {
    for (size_t i = 0; i < last_alloc; ++i) { allocated[i] = 0; }
    for (size_t i = 0; i < last_dealloc; ++i) { deallocated[i] = 0; }
}

void print_mem_summary(void) {
#ifdef MEM_TEST
    puts("Allocated:");
    for (size_t i = 0; i < last_alloc; ++i) { printf("  %lx\n", allocated[i]); }
    puts("Deallocated:");
    for (size_t i = 0; i < last_dealloc; ++i) { printf("  %lx\n", deallocated[i]); }
    check_all_freed();
#endif
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

void lam_free_error(void) { puts("error freind ptr "); }

void lam_free(void* ptr) {
#ifdef MEM_TEST
    assert(last_dealloc < MEM_SZ);
    uintptr_t to_free = (uintptr_t)ptr;
    deallocated[last_dealloc++] = to_free;
    for (size_t i = 0; i < last_alloc; ++i) {
        if (to_free == allocated[i]) { return; }
    }
    lam_free_error();
#endif
    free(ptr);
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

