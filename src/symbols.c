#include <mem.h>
#include <symbols.h>

int stringTableInitWithSize(StringTable st[static 1], size_t size, size_t max_tries);
void stringTableSearch(StringTable m[static 1], Lstr k, Result res[static 1]) ;
bool stringTableContains(StringTable* m, Lstr k);
void stringTableInsert(StringTable* m, Lstr k, Lterm* t, Result* res);

StringTable _symbols = {0};

int initialize_symbol_table(void) {
    return stringTableInitWithSize(&_symbols, 100, 17);
}

void free_symbol_table() {
    if (_symbols.table) {
        for (unsigned long i = 0; i < _symbols.size; ++i) {
            Entry* e = _symbols.table + i;
            if (e->k.s) {
                lam_free_term(e->t);
                lam_free((void*)e->k.s);
            }
        }
        free(_symbols.table);
    }
}

bool lam_str_name_is_defined(Lstr name) {
    if (_symbols.table) {
        return stringTableContains(&_symbols, name);
    }
    return false;
}

bool lam_name_is_defined(const char* name) {
    if (_symbols.table) {
        return stringTableContains(&_symbols, lam_str(name));
    }
    return false;
}

int lam_str_name_insert(Lstr name, Lterm* t) {
    Result r = {0};
    stringTableInsert(&_symbols, name, t, &r);
    return r.err;
}

int lam_name_insert(const char* name, Lterm* t) {
    Result r = {0};
    stringTableInsert(&_symbols, lam_str(name), t, &r);
    return r.err;
}

Lterm* lam_symbols_search_clone(Lstr name) {
    if (_symbols.table) {
        Result r;
        stringTableSearch(&_symbols, name, &r);
        return r.err ? 0x0 : lam_clone(((Entry*)r.ok)->t);
    }
    return 0x0;
}

Lterm* lam_name_search(const char* name) {
    if (_symbols.table) {
        Result r;
        stringTableSearch(&_symbols, lam_str(name), &r);
        return r.err ? 0x0 : ((Entry*)r.ok)->t;
    }
    return 0x0;
}

unsigned long string_hash(const unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

int stringTableInitWithSize(StringTable st[static 1], size_t size, size_t max_tries) {
    Entry* table = calloc(size, sizeof (Entry));
    if (!table) {
        fprintf(stderr, "NOt memory to initialize table!");
        return -1;
    }
    *st = (StringTable) {
        .table = table, .size = size, .inserts = 0, .max_tries = max_tries
    };
    return 0;
}

void stringTableSearch(StringTable m[static 1], Lstr k, Result res[static 1]) {
    *res = (Result) { .ok=0x0, .err=true };

    if (k.len == 0 || k.s == NULL) {
        fprintf(stderr, "Error: null key\n");
        return;
    }

    unsigned int h = string_hash((const unsigned char*)k.s) % m->size;
    unsigned int nmovs = 0;
    while (nmovs < m->max_tries) {

        Entry* e = &m->table[h];
        if (e->k.s == NULL || lam_str_eq(e->k, k)) {
            *res = (Result) { .ok=e, .err=false };
            return;
        } 
        h = (h + 7) % m->size;
        nmovs++;
    }
}

bool stringTableContains(StringTable* m, Lstr k) {
    if (k.len == 0 || k.s == NULL) {
        fprintf(stderr, "Error: null key\n");
        return false;
    }

    unsigned int h = string_hash((const unsigned char*)k.s) % m->size;
    unsigned int nmovs = 0;
    while (nmovs < m->max_tries) {

        Entry* e = &m->table[h];
        if (e->k.s == NULL) {
            return false;
        } else if (lam_str_eq(e->k, k)) {
            return true;
        }
        h = (h + 7) % m->size;
        nmovs++;
    }
    return false;
}

void stringTableInsert(StringTable* m, Lstr k, Lterm* t, Result* res) {
    if (k.len == 0 || k.s == NULL) {
        fprintf(stderr, "Error: null key\n");
        res->err = true;
        return;
    }

    unsigned int h = string_hash((const unsigned char*)k.s) % m->size;
    unsigned int nmovs = 0;
    while (nmovs < m->max_tries) {

        Entry* e = &m->table[h];
        if (e->k.s == NULL) {
            if (++m->inserts > m->size * 65 / 100) {
                fprintf(stderr, "Too many keys for map!\n");
                res->err = true;
                return;
            }

            e->k = (Lstr){.s=k.s, .len=k.len};
            e->t = t;
            res->ok = e;
            res->err = false;
            return;
        } else if (lam_str_eq(e->k, k)) {
            res->ok = e;
            res->err = false;
            return;
        }
        h = (h + 7) % m->size;
        nmovs++;
    }
    fprintf(stderr, "COuld not insert key in table!\n");
}

