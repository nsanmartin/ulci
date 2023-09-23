%code requires {
    #include <stdio.h>
    #include <lam.h>
    #include <parser-util.h>


}

%union {
    char* sval;
    Lterm* termval;
}


%token DOT
%token LPAREN
%token RPAREN
%token LAMBDA
%token <sval> VAR
%token EOL

%nterm <termval> expression
%nterm <termval> app_expr
%nterm <termval> simple_expr

%%

expression_list:
    expression EOL { set_last_lam_term($1); }
    | expression_list expression EOL { set_last_lam_term($2); }
    ;

expression:
    LAMBDA VAR DOT expression {
        Lterm* abs = lam_new_abs(lam_str($2), $4);
        $$ = abs;
    }
    | app_expr { $$ = $1; }
    ;

app_expr:
    simple_expr { $$ = $1; }
    | app_expr simple_expr {
        Lterm* app = lam_new_app($1, $2);
        $$ = app;
    }
    ;

simple_expr:
    VAR {
        Lterm* var = lam_new_var(lam_str($1));
        $$ = var;
    }
    | LPAREN expression RPAREN {
        $$ = $2;
    }
    ;

%%

#ifndef LAM_LIB_PARSER
int main (void) {
    yyin = stdin;
    yyparse();
}
#endif
