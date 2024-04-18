#ifndef __MEM_H_
#define __MEM_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <str.h>

void* lam_malloc(size_t size) ;
void* lam_calloc(size_t nmemb, size_t size);
void lam_free_mem(void);
char* lam_strndup(const char* s, size_t n);
//char* lam_strdup(const char* s) ;

void lam_free(void* ptr);
//static inline char* lam_strdup(const char* s) { return strdup(s); }
char* lam_strdup(const char* s);

void print_mem_summary(void) ;
#endif

