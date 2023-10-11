#ifndef __PARSER_LAM_READER_H__
#define __PARSER_LAM_READER_H__

#include <stdio.h>
#include "lam.h"

#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */

extern FILE* yyin;

extern int yylex(void);
extern void yyerror(const char*);

void set_input_string(const char* in) ;
void end_lexical_scan(void) ;

void yyerror(const char* s) ;
int yyparse(void) ;


void set_input_string(const char* in);
void end_lexical_scan(void);

Lstr parse_string(const char* in, Lstr (*to_str)(const Lterm[static 1])) ;
Lstr eval_string(const char* in, Lstr (*to_str)(const Lterm[static 1])) ;

int set_last_lam_term(Lterm t[static 1]) ;
Lterm* get_lam_term(void) ;

void set_input_string(const char* in);
void end_lexical_scan(void);

int parser_read_expression(Lterm t[static 1]) ;
void parser_set_repl_fn(void) ;
#endif
