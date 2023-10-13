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

%nterm <termval> expression
%nterm <termval> not_lambda
%nterm <termval> neither_lambda_nor_app

%%

expression_list
    : expression EOL { parser_read_expression($1); }
    | expression_list expression EOL { parser_read_expression($2); }
    ;

expression
    : LAMBDA VAR DOT expression {
        Lterm* abs = lam_new_abs(lam_str($2), $4);
        $$ = abs;
    }
    | not_lambda { $$ = $1; }
    ;

not_lambda
    : neither_lambda_nor_app { $$ = $1; }
    | not_lambda neither_lambda_nor_app {
        Lterm* app = lam_new_app($1, $2);
        $$ = app;
    }
    ;

neither_lambda_nor_app
    : VAR {
        Lterm* var = lam_new_var(lam_str($1));
        $$ = var;
    }
    | LPAREN expression RPAREN {
        $$ = $2;
    }
    ;

%%

