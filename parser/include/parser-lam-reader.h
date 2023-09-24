#ifndef __PARSER_LAM_READER_H__
#define __PARSER_LAM_READER_H__
#include <stdio.h>
#include "lam.h"
extern FILE* yyin;

extern int yylex(void);
extern void yyerror(const char*);

void set_input_string(const char* in) ;
void end_lexical_scan(void) ;

void yyerror(const char* s) ;
int yyparse(void) ;


void set_input_string(const char* in);
void end_lexical_scan(void);

Lstr parse_string(const char* in) ;

int set_last_lam_term(Lterm t[static 1]) ;
Lterm* get_lam_term(void) ;

void set_input_string(const char* in);
void end_lexical_scan(void);

int parser_read_expression(Lterm t[static 1]) ;
#endif
