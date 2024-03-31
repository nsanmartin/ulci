%{
#include <stdio.h>
#include "parser.tab.h"
#include <parser-names.h>

const char* TokenNames[] = {
    [DOT] = "DOT",
    [LPAREN] = "(",
    [RPAREN] = ")",
    [LAMBDA] = "LAMBDA",
    [VAR] = "VAR",
    [NAME] = "NAME",
    [EQUALS] = "=",
    [SET] = "SET",
    [EOL] = "EOL\n",
    [SEMICOLON] = "SEMICOLON"
};

static bool last_was_end = false;

static inline
enum yytokentype id_token(enum yytokentype t) {
    last_was_end = t == EOL || t == SEMICOLON;
    return t;
}

static inline
enum yytokentype print_return(enum yytokentype t) {
    printf("%s ", TokenNames[t]);
    return t;
}

#define read_token(Tk) id_token(Tk)
// #define read_token(Tk) return Tk

%}

%option noyywrap
%option nounput
%option noinput

%%
[ \t]                   { continue; }
\n                      { if (!last_was_end) return read_token(EOL); }
";"                     { if (!last_was_end) return read_token(SEMICOLON); }
"#".*                   { continue; }
"."                     { return read_token(DOT);}
"("                     { return read_token(LPAREN);}
")"                     { return read_token(RPAREN);}
"\\"                    { return read_token(LAMBDA);}
"="                     { return read_token(EQUALS);}
"set"                   { return read_token(SET);}
[a-zA-Z][a-zA-Z0-9_]*   {
    char* text = lam_strdup(yytext);
    yylval.sval = text;
    if (lam_name_is_defined(text)) {
        //TODO: return NAME as term
        return read_token(NAME);
    } else {
        return read_token(VAR);
    }
}

%%

void set_input_string(const char* in) {
  yy_scan_string(in);
}

void end_lexical_scan(void) {
  yy_delete_buffer(YY_CURRENT_BUFFER);
}

