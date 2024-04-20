#ifndef __LAM_STR_H_
#define __LAM_STR_H_

#include <stdbool.h>
#include <stdio.h>

#include <mem.h>

typedef struct { const char* s; size_t len; } Lstr;

typedef struct LstrList { Lstr s; struct LstrList* next; } LstrList;

void freeLstrList(LstrList* ls);
//char* lam_strndup(const char* s, size_t n);

static inline void lam_str_fwrite(const Lstr s) { fwrite(s.s, 1, s.len, stdout); }

static inline Lstr LEMPTY_STR(void) {
    return  (Lstr){.s="",.len=0};
}

static inline char* lam_str_to_cstr(Lstr s) {
    return (char*)s.s;
}

static inline Lstr lam_strn_view(const char* s, size_t len) {
    return (Lstr){.s=s, .len=len};
}

//TODO: rename to lam_str_view
static inline Lstr lam_str(const char* s) {
    return (Lstr){.s=s, .len=strlen(s)};
}

static inline Lstr lam_lstrndup(const char* s, size_t n) {
    return (Lstr){.s=lam_strndup(s,n), .len=n};
}

static inline Lstr lam_lstr_dup(Lstr s) {
    return (Lstr){.s=_lam_strndup(s.s, s.len, __func__), .len=s.len};
}

static inline Lstr lam_str_dup(const char* s) {
    size_t n = strlen(s);
    return (Lstr){.s=_lam_strndup(s,n, __func__), .len=n};
}

static inline bool lam_str_null(Lstr s) {
    return !s.s;
}

static inline int lam_str_eq(Lstr s, Lstr t) {
    return s.len == t.len && strncmp(lam_str_to_cstr(s), lam_str_to_cstr(t), s.len) == 0;
}

static inline size_t lam_strlen(Lstr s) {
    return strlen(s.s);
}


#endif
