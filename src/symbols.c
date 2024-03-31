#include <mem.h>
#include <lam-symbols.h>

int stringTableInitWithSize(StringTable* st, size_t size, size_t max_tries);
void stringTableSearch(StringTable* m, Lstr k, Result* res) ;
bool stringTableContains(StringTable* m, Lstr k);
void stringTableInsert(StringTable* m, Lstr k, Lterm* t, Result* res);

StringTable _symbols = {0};

int initialize_symbol_table(void) {
    return stringTableInitWithSize(&_symbols, 100, 17);
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

int lam_name_insert(const char* name, Lterm* t) {
    Result r;
    stringTableInsert(&_symbols, lam_str(name), t, &r);
    return r.err;
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

int stringTableInitWithSize(StringTable* st, size_t size, size_t max_tries) {
    Entry* table = lam_malloc(size * (sizeof (Entry)));
    if (!table) {
        fprintf(stderr, "NOt memory to initialize table!");
        return -1;
    }
    *st = (StringTable) {
        .table = table, .size = size, .inserts = 0, .max_tries = max_tries
    };
    return 0;
}

void stringTableSearch(StringTable* m, Lstr k, Result* res) {
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
