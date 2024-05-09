#ifndef __LAM_LEXER_H_
#define __LAM_LEXER_H_

#include <stdlib.h>
#include <stdio.h>

enum { LamMaxKeywordLen = 255 };
typedef struct {
    char s[LamMaxKeywordLen];
    size_t len;
    size_t col;
    size_t read_so_far;
    // rename to LamLexerCtx
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
    LErrorInputTooLarge,
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
static inline size_t lam_get_ncol(LamKeywordBuf buf[static 1]) { return buf->col; }
#endif
