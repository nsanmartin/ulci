%{
#include <stdio.h>
#include "parser.tab.h"
#include <parser-names.h>
%}

%option noyywrap
%option nounput
%option noinput

%%
[ \t]                   { continue; }
"#".*                   { continue; }
"."                     { return DOT; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"\\"                    { return LAMBDA; }
"="                     { return EQUALS; }
"set"                   { return SET; }
[a-zA-Z][a-zA-Z0-9_]*   {
    char* text = strdup(yytext);
    yylval.sval = text;
    if (lam_name_is_defined(text)) {
        //TODO: return NAME as term
        return NAME;
    } else {
        return VAR;
    }
}
[\n;]                    { return EOL; }

%%

void set_input_string(const char* in) {
  yy_scan_string(in);
}

void end_lexical_scan(void) {
  yy_delete_buffer(YY_CURRENT_BUFFER);
}

