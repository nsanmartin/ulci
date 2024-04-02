#ifndef __LAM_STR_H_
#define __LAM_STR_H_

typedef struct { const char* s; size_t len; } Lstr;

typedef struct LstrList { Lstr s; struct LstrList* next; } LstrList;

static inline Lstr LEMPTY_STR() {
    return  (Lstr){.s="",.len=0};
}

static inline char* lam_str_to_cstr(Lstr s) {
    return (char*)s.s;
}

static inline Lstr lam_strn(const char* s, size_t len) {
    return (Lstr){.s=s, .len=len};
}

static inline Lstr lam_str(const char* s) {
    return (Lstr){.s=s, .len=strlen(s)};
}

static inline Lstr lam_strndup(const char* s, size_t n) {
    return (Lstr){.s=strndup(s,n), .len=n};
}

static inline Lstr lam_str_dup(const char* s) {
    size_t n = strlen(s);
    return (Lstr){.s=strndup(s,n), .len=n};
}

static inline bool lam_str_null(Lstr s) {
    return !s.s;
}

static inline int lam_str_eq(Lstr s, Lstr t) {
    return s.len == t.len && strncmp(lam_str_to_cstr(s), lam_str_to_cstr(t), s.len) == 0;
}

//TODO delete?
//static inline int lam_strcmp(Lstr s, Lstr t) {
//    return strcmp(lam_str_to_cstr(s), lam_str_to_cstr(t));
//}

static inline size_t lam_strlen(Lstr s) {
    return strlen(s.s);
}


#endif
