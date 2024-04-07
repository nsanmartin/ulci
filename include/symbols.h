#ifndef __LAM_SYMBOLS__
#define __LAM_SYMBOLS__

#include <stdbool.h>

#include <lam.h>


typedef struct {
    unsigned int v;
    Lterm* t;
    Lstr k;
} Entry;

typedef struct {
    Entry* table;
    unsigned long size;
    size_t inserts;
    size_t max_tries;
} StringTable;

typedef struct {
    bool err;
    void* ok;
} Result;

unsigned long hash(unsigned char *str);

bool lam_name_is_defined(const char* name);
bool lam_str_name_is_defined(Lstr name);

int initialize_symbol_table(void);
int lam_name_insert(const char* name, Lterm* t);
int lam_str_name_insert(Lstr name, Lterm* t) ;
Lterm* lam_name_search(const char* name);
Lterm* lam_symbols_search_clone(Lstr name);

#endif
