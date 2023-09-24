// #include "parser.tab.c"
#include <stdio.h>

#include "parser-lam-reader.h"

void test_parse_impl(const char* s, const char* out, const char* expected) {
    //printf("parsing '%s' => %s\n", s, r);
    if (strcmp(out, expected)) {
        fprintf(stderr, RED "Error parsing" RESET " %s:\n  %s\n  %s\n", s, out, expected);
    } else {
        printf(GREEN "Ok" RESET "\n");
    }
}

#define test_parse_many_paren(S, R)                                   \
    test_parse_impl(S, parse_string( S "\n", lam_term_to_str_more_paren).s, R);

#define test_parse(S,R)                                                 \
    test_parse_impl(S, parse_string( S "\n", lam_term_to_str_less_paren).s, R);

int main (void) {
    puts("Many parents");
    test_parse_many_paren("x (y z)", "(x (y z))");
    test_parse_many_paren("(x (y z))", "(x (y z))");
    test_parse_many_paren("x", "x");
    test_parse_many_paren("(x x)", "(x x)");
    test_parse_many_paren("(x (y z))", "(x (y z))");
    test_parse_many_paren("(\\x.(x x))", "(\\x.(x x))");
    test_parse_many_paren("(\\x.(y x))", "(\\x.(y x))");
    test_parse_many_paren("((\\x.x) (\\x.y))", "((\\x.x) (\\x.y))");
    test_parse_many_paren(
        "(\\x.((y (\\x.xx)) ((\\x.y) (\\x.xy))))",
        "(\\x.((y (\\x.xx)) ((\\x.y) (\\x.xy))))"
    );


    puts("Less parents");
    test_parse("x", "x");
    test_parse("x x", "x x");
    test_parse("x (y z)", "x (y z)");
    test_parse("x x x x", "x x x x");
    test_parse("x (x x) x", "x (x x) x");
    test_parse("(\\x.x x)", "\\x.x x");
    test_parse("(\\x.x) (\\x.y)", "(\\x.x) (\\x.y)") 
    test_parse(
        "\\x.y (\\x.x) (\\x.y) (\\x.y)",
        "\\x.y (\\x.x) (\\x.y) (\\x.y)"
    );
    test_parse("(\\x.x x x x)", "\\x.x x x x");
    test_parse(
        "(\\x.y (\\x.x) ((\\x.y) (\\x.y)))",
        "\\x.y (\\x.x) ((\\x.y) (\\x.y))"
    );
    test_parse("\\x.y (\\z.x)", "\\x.y (\\z.x)");
}
