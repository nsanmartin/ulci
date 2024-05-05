#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <lexer.h>
#include <str.h>
#include <symbols.h>


const char* LamTokenNames[] = {
    [LDot]       = "Dot",
    [LLparen]    = "Lparen",
    [LRparen]    = "Rparen",
    [LLambda]    = "Lambda",
    [LVar]       = "Var",
    [LName]      = "Name",
    [LSet]       = "Set",
    [LEquals]    = "=",
    [LEol]       = "Eol\n",
    [LSemicolon] = ";",
    [LEof]       = "EOF",
    [LError]     = "Error",
};


typedef struct {
    const char* buf;
    size_t ix;
    size_t len;
} StrIter;

// Scan from string:
StrIter _lam_scan_str_iter = { .buf=0x0, .ix=0, .len=0 };

char lam_getc_str(LamKeywordBuf buf[static 1]) {
    if (_lam_scan_str_iter.ix <  _lam_scan_str_iter.len) {
        ++buf->col;
        return _lam_scan_str_iter.buf[_lam_scan_str_iter.ix++];
    }
    return '\0';
}

int lam_ungetc_str(LamKeywordBuf buf[static 1], char c) {
    if (c) {
        char d = _lam_scan_str_iter.buf[--_lam_scan_str_iter.ix];
        if (d != c) {
            puts("Unget error, must match (TODO change this?)");
            exit(1);
        }
        --buf->col;
    }
    return c;
}


// Scan from file:
FILE* _lam_scan_file = 0x0;
char lam_getc_file(LamKeywordBuf buf[static 1]) { ++buf->col; return fgetc(_lam_scan_file); }
int lam_ungetc_file(LamKeywordBuf buf[static 1], char c) { --buf->col; return ungetc(c, _lam_scan_file); }
//TODO: avoid using global state and use thelexer context instead


///
// change this fn to switch between reading from buf or file
char (*_lam_getc)(LamKeywordBuf buf[static 1]) = lam_getc_str;
int (*_lam_ungetc)(LamKeywordBuf buf[static 1], char c) = lam_ungetc_str;

void lam_scan_set_file_input(FILE* fp) {
    _lam_scan_file = fp;
    _lam_getc = lam_getc_file;
    _lam_ungetc = lam_ungetc_file;
}

void lam_scan_set_str_input(const char* buf) {
    _lam_scan_str_iter.buf = buf;
    _lam_scan_str_iter.ix = 0;
    _lam_scan_str_iter.len = strlen(buf); 
}

const char* lam_token_to_str(LamTokenTag tk) {
    if (tk >= LamTokenTagsLen) { return 0x0; }
    return LamTokenNames[tk];
}

LamTokenTag lam_scan_next(LamKeywordBuf buf[static 1]) {
    char c = _lam_getc(buf);
    for (; isblank(c) ; c = _lam_getc(buf))
        ;
    switch(c) {
        case '.': return LDot;
        case '(': return LLparen;
        case ')': return LRparen;
        case '\\': return LLambda;
        case '=': return LEquals;
        case '\n': return LEol;
        case ';': return LSemicolon;
        case EOF: return LEof;
        case '\0': return LEof;

    }
    if (!isalpha(c) && c != '_') {
        return LError;
    }
    buf->len = 0;
    for (;;) {
        assert_keyword_len_is_valid(buf->len);
        buf->s[buf->len++] = c;
        c = _lam_getc(buf);
        if (!isalnum(c) && c != '_') {
            _lam_ungetc(buf, c);
            break;
        }
    }
    
    if (buf->len == 3 && strncmp("set", buf->s, 3) == 0) {
        return LSet;
    }
    if (lam_str_name_is_defined(lam_strn_view(buf->s, buf->len))) {
        return LName;
    }
    return LVar;
}

