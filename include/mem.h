#ifndef __MEM_H_
#define __MEM_H_

#include <stddef.h>
#include <stdlib.h>

void* lam_malloc(size_t size) ;
void lam_free_mem(void);
char* lam_strdup(const char* s) ;

static inline void lam_free(void* ptr) { free(ptr); }

#endif

