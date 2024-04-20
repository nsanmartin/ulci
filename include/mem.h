#ifndef __MEM_H_
#define __MEM_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

//#define MEM_TEST

#ifdef MEM_TEST
#define lam_malloc(S) _lam_malloc(S, __func__)
#define lam_calloc(NM, S) _lam_calloc(NM, S, __func__)
#define lam_strdup(STR) _lam_strdup(STR, __func__)
#define lam_strndup(STR ,SZ) _lam_strndup(STR, SZ, __func__)
#else
#define lam_malloc(S) _lam_malloc(S, __func__)
#define lam_calloc(NM, S) _lam_calloc(NM, S, __func__)
#define lam_strdup(STR) _lam_strdup(STR, __func__)
#define lam_strndup(STR ,SZ) _lam_strndup(STR, SZ, __func__)
#endif

void* _lam_malloc(size_t size, const char* caller) ;
void* _lam_calloc(size_t nmemb, size_t size, const char* caller);
char* _lam_strdup(const char* s, const char* caller);
char* _lam_strndup(const char* s, size_t n, const char* caller) ;

void lam_free(void* ptr);
void lam_free_mem(void);

void print_mem_summary(void) ;
#endif

