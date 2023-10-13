%code requires {
    #include <stdio.h>
    #include <lam.h>
    #include <parser-lam-reader.h>
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

// %nterm <termval> sub_expr
// %nterm <termval> app_expr
%nterm <termval> simple_expr
%nterm <termval> abstraction
%nterm <termval> application
%nterm <termval> variable
%nterm <termval> expression

%%

expression_list: %empty | expression_list expression EOL ;

expression
    : simple_expr { parser_read_expression($1); $$ = $1; }
    | application { parser_read_expression($1); $$ = $1; }
    ;

abstraction
    : LAMBDA VAR DOT simple_expr {
        Lterm* abs = lam_new_abs(lam_str($2), $4);
        $$ = abs;
    }
    ;

application
    : expression simple_expr {
        Lterm* app = lam_new_app($1, $2);
        $$ = app;
    }
    ;

variable
    : VAR {
        Lterm* var = lam_new_var(lam_str($1));
        $$ = var;
    }
    ;

simple_expr
    : variable
    | abstraction
    | LPAREN expression RPAREN { $$ = $2; }
    ;

%%

