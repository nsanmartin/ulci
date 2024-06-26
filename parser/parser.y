%code requires {

/* 
 * This is the grammar for bison. src/recursive-descendant.c contains
 * the custom parser which I wrote because I was having an error I 
 * was not able to debug easily evaluating some lambda terms.
 * Unfortunately, I could not reproduce that bug in that grammar so I
 * did not fix bison's one. Since, I am only using the custom one.
*/

    #include <stdio.h>
    #include <lam.h>
    #include <parser-lam-reader.h>
    #include <symbols.h>
    #define YYDEBUG 1
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
%token <sval> NAME
%token SET
%token EQUALS
%token EOL
%token SEMICOLON

%nterm <termval> expression
%nterm <termval> variable
%nterm <termval> not_lambda
%nterm <termval> neither_lambda_nor_app

%define parse.error verbose

%%

statement_list
    : statement_list statement
    | %empty
    ;
statement
    : SET VAR EQUALS expression end {
        Lterm* t = $4;
        parser_read_expression(t);
        lam_name_insert($2, t);
    }
    | expression end { parser_read_expression($1); }
    ;
expression
    : LAMBDA VAR DOT expression {
        Lterm* abs = lam_new_abs(lam_str($2), $4);
        $$ = abs;
    }
    | not_lambda { $$ = $1; }
    ;
end : SEMICOLON  | EOL | YYEOF ;
not_lambda
    : neither_lambda_nor_app { $$ = $1; }
    | not_lambda neither_lambda_nor_app {
        Lterm* app = lam_new_app($1, $2);
        $$ = app;
    }
    ;

neither_lambda_nor_app
    : variable
    | LPAREN expression RPAREN { $$ = $2; }
    | NAME {
        Lterm* t = lam_name_search($1);
        $$ = t;
    }
    ;

variable
    : VAR {
        Lterm* var = lam_new_var(lam_str($1));
        $$ = var;
    }
    ;
%%

