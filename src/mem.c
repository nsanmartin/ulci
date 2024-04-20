#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mem.h>

#define MEM_SZ 1000000

typedef struct {
    uintptr_t  p;
    const char* c;
} PtrCaller;

PtrCaller allocated[MEM_SZ] = {0};
PtrCaller deallocated[MEM_SZ] = {0};

size_t last_alloc = 0;
size_t last_dealloc = 0;

void push_allocd(uintptr_t p, const char* c, size_t i) {
    //printf("caller: %s, ptr: %lx\n", c, p);
    allocated[i] = (PtrCaller){.p=p, c=c}; 
}

void check_all_freed() {
    for (size_t i = 0; i < last_alloc; ++i) {
        PtrCaller allocd = allocated[i];

        size_t j = 0; 
        for (; j < last_dealloc; ++j) {
            uintptr_t deallocd =  deallocated[j].p;
            if (allocd.p == deallocd) {
                //printf("%lx ok\n", allocd.p);
                break;
            }
        }
        if (j == last_dealloc) {
            const char* fn =  allocd.c;
            printf("%lx Not Freed!! fn: %s\n", allocd.p, fn ? fn : "unknown");
        }
    }
}

void clear_allocated() {
    for (size_t i = 0; i < last_alloc; ++i) { allocated[i].p = 0; }
    for (size_t i = 0; i < last_dealloc; ++i) { deallocated[i].p = 0; }
}

void print_mem_summary(void) {
#ifdef MEM_TEST
    //puts("Allocated:");
    //for (size_t i = 0; i < last_alloc; ++i) { printf("  %lx\n", allocated.p[i]); }
    //puts("Deallocated:");
    //for (size_t i = 0; i < last_dealloc; ++i) { printf("  %lx\n", deallocated.p[i]); }
    check_all_freed();
#endif
}

void* _lam_malloc(size_t size, const char* caller) {
#ifdef MEM_TEST
    void* r = malloc(size);
    assert(last_alloc < MEM_SZ);
    push_allocd((uintptr_t)r, caller, last_alloc++);
    return r;
#else
    (void)caller;
    return malloc(size);
#endif
}

void* _lam_calloc(size_t nmemb, size_t size, const char* caller) {
#ifdef MEM_TEST
    void* r = calloc(nmemb, size);
    assert(last_alloc < MEM_SZ);
    push_allocd((uintptr_t)r, caller, last_alloc++);
    return r;
#else
    (void)caller;
    return calloc(nmemb, size);
#endif
}

void lam_free_error(void) { puts("error freind ptr "); }

void lam_free(void* ptr) {
#ifdef MEM_TEST
    assert(last_dealloc < MEM_SZ);
    uintptr_t to_free = (uintptr_t)ptr;
    deallocated[last_dealloc++].p = to_free;
    for (size_t i = 0; i < last_alloc; ++i) {
        if (to_free == allocated[i].p) { return; }
    }
    lam_free_error();
#endif
    free(ptr);
}

char* _lam_strdup(const char* s, const char* caller) {
#ifdef MEM_TEST
    assert(last_alloc < MEM_SZ);
    char* r = strdup(s);
    push_allocd((uintptr_t)r, caller, last_alloc++);
    return r;
#else
    (void)caller;
    return strdup(s);
#endif
}

char* _lam_strndup(const char* s, size_t n, const char* caller) {
#ifdef MEM_TEST
    assert(last_alloc < MEM_SZ);
    char* r = strndup(s,n);
    push_allocd((uintptr_t)r, caller, last_alloc++);
    return r;
#else
    (void)caller;
    return strndup(s,n);
#endif
}

void lam_free_mem(void) { }

