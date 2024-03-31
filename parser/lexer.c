#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <parser-lam-reader.h>
#include <parser-names.h>


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
    LError
} LamTokenTag;


typedef struct {
    const char* buf;
    size_t ix;
    size_t len;
} StrIter;

// Scan from string:
char s0[] = "x = \\x.x y";
StrIter _lam_scan_str_iter = {
    .buf=s0,
    .ix=0,
    .len=sizeof(s0)-1
};

char lam_getc_str(void) {
    if (_lam_scan_str_iter.ix <  _lam_scan_str_iter.len) {
        return _lam_scan_str_iter.buf[_lam_scan_str_iter.ix++];
    }
    return '\0';
}
int lam_ungetc_str(char c) {
    if (c) {
        char d = _lam_scan_str_iter.buf[--_lam_scan_str_iter.ix];
        if (d != c) {
            puts("Unget error, must match (TODO change this?)");
            exit(1);
        }
    }
    return c;
}


// Scan from file:
FILE* _lam_scan_file = 0x0;
char lam_getc_file(void) { return fgetc(_lam_scan_file); }
int lam_ungetc_file(char c) { return ungetc(c, _lam_scan_file); }


///
// change this fn to switch between reading from buf or file
char (*_lam_getc)(void) = lam_getc_str;
int (*_lam_ungetc)(char c) = lam_ungetc_str;

void lam_scan_set_fileinput(FILE* fp) {
    _lam_scan_file = fp;
    _lam_getc = lam_getc_file;
    _lam_ungetc = lam_ungetc_file;
}

const char* lam_token_to_str(LamTokenTag tk) {
    switch (tk) {
        case LDot: return "Dot";
        case LLparen: return "Lparen";
        case LRparen: return "Rparen";
        case LLambda: return "Lambda";
        case LVar: return "Var";
        case LName: return "Name";
        case LSet: return "Set";
        case LEquals: return "Equals";
        case LEol: return "Eol";
        case LEof: return "EOF";
        case LSemicolon: return "Semicolon";
        case LError: return "Lexical error";
        default: {
             puts("error scanning");
             exit(1);
         }
    }
}

LamTokenTag lam_scan_next(LamKeywordBuf buf[static 1]) {
    char c = _lam_getc();
    for (; isblank(c) ; c = _lam_getc())
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
    for (;;) {
        buf->s[buf->len++] = c;
        c = _lam_getc();
        if (!isalnum(c) && c != '_') {
            _lam_ungetc(c);
            break;
        }
    }
    
    if (buf->len == 3 && strncmp("set", buf->s, 3) == 0) {
        return LSet;
    }

    if (lam_str_name_is_defined(lam_strn(buf->s, buf->len))) {
        return LName;
    } 
    return LVar;
}

int main (int argc, char* argv[]) {
    if(initialize_symbol_table()) {
        fprintf(stderr, "Erroe: not memory to initialize parser\n");
        return -1;
    }
    lam_scan_set_fileinput(stdin);
    printf("> ");
    for(;;) {
        LamKeywordBuf buf = {0};
        LamTokenTag tk = lam_scan_next(&buf);
        if (tk == LEof) { break; }
        if (tk == LVar) {
            fwrite("Var(", 1, 4, stdout);
            fwrite(buf.s, 1, buf.len, stdout);
            fwrite(") ", 1, 2, stdout);
        } else if (tk == LName) {
            fwrite("Var(", 1, 4, stdout);
            fwrite(buf.s, 1, buf.len, stdout);
            fwrite(") ", 1, 2, stdout);
        } else if (tk == LEol) {
            printf("\n> ");
        } else {
            printf("%s ", lam_token_to_str(tk));
        }
    }


    //if (argc == 1) {
    //    //interactive();
    //} else {
    //    for (int i = 1; i < argc; ++i) {
    //        yyin = fopen(argv[i], "r");
    //        if (yyin) {
    //            yylex();
    //        }
    //    }
    //}
}

char *readFile(char *fileName)
{
    FILE *file = fopen(fileName, "r");
    char *code;
    size_t n = 0;
    int c;

    if (file == NULL)
        return NULL; //could not open file

    code = malloc(1000);

    while ((c = fgetc(file)) != EOF)
    {
        code[n++] = (char) c;
    }

    // don't forget to terminate with the null character
    code[n] = '\0';

    return code;
}


