#ifndef __LAM_LEXER_H_
#define __LAM_LEXER_H_

#include <stdlib.h>
#include <stdio.h>

enum { LamMaxKeywordLen = 256 };
typedef struct {
    char s[LamMaxKeywordLen];
    size_t len;
} LamKeywordBuf;

typedef enum {
    LDot,
    LLparen,
    LRparen,
    LLambda,
    LVar,
    LName,
    LSet,
    LEquals,
    LEol,
    LSemicolon,
    LEof,
    LError,
    LamTokenTagsLen
} LamTokenTag;

LamTokenTag lam_scan_next(LamKeywordBuf buf[static 1]);

const char* lam_token_to_str(LamTokenTag tk);
void lam_scan_set_file_input(FILE* fp);
void lam_scan_set_str_input(const char* buf);
static inline void assert_keyword_len_is_valid(size_t l) { 
    if(l >= LamMaxKeywordLen) { puts("Lam: keyword too large, aborting."); exit(EXIT_FAILURE); }
}
#endif
