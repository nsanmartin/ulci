// #include "parser.tab.c"
#include <stdio.h>

#include "parser-lam-reader.h"

int test_parse_impl(const char* s, const char* out, const char* expected) {
    if (strcmp(out, expected)) {
        fprintf(
            stderr,
            RED "Error parsing" RESET "\n"
            "  test\t %s\n  res\t %s\n  exp\t %s\n",
            s, out, expected);
        return -1;
    } else {
        printf("%s " GREEN "Ok" RESET "\n", s);
        return 0;
    }
}

void test_parse_all_paren(const char* test, const char* expected) {
    if(test_parse_impl(
        test, parse_string(test, lam_term_to_str_more_paren).s, expected
    )) {
        exit(1);
    };
}

void test_parse_convention(const char* test, const char* expected) {
    if(test_parse_impl(
        test, parse_string(test, lam_term_to_str_less_paren).s, expected
    )) {
        exit(1);
    };
}

void test_eval_all_paren(const char* test, const char* expected) {
    if(test_parse_impl(
        test, eval_string(test, lam_term_to_str_more_paren).s, expected
    )) {
        exit(1);
    };
}

void test_eval_convention(const char* test, const char* expected) {
    if(test_parse_impl(
        test, eval_string(test, lam_term_to_str_less_paren).s, expected
    )) {
        exit(1);
    };
}

int main (void) {
    puts("Many parents");
    test_parse_all_paren("x (y z);", "(x (y z))");
    test_parse_all_paren("(x (y z));", "(x (y z))");
    test_parse_all_paren("x;", "x");
    test_parse_all_paren("(x x);", "(x x)");
    test_parse_all_paren("(x (y z));", "(x (y z))");
    test_parse_all_paren("(\\x.(x x));", "(\\x.(x x))");
    test_parse_all_paren("(\\x.(y x));", "(\\x.(y x))");
    test_parse_all_paren("((\\x.x) (\\x.y));", "((\\x.x) (\\x.y))");
    test_parse_all_paren(
        "(\\x.((y (\\x.xx)) ((\\x.y) (\\x.xy))));",
        "(\\x.((y (\\x.xx)) ((\\x.y) (\\x.xy))))"
    );
    test_parse_all_paren("\\x.\\y.x y;", "(\\x.(\\y.(x y)))");
    test_parse_all_paren(
        "\\f.(\\u.f(u u))(\\v.f(v v));",
        "(\\f.((\\u.(f (u u))) (\\v.(f (v v)))))"
    );

    puts("Less parents");
    test_parse_convention("x;", "x");
    test_parse_convention("x x;", "x x");
    test_parse_convention("x (y z);", "x (y z)");
    test_parse_convention("(\\x.x x);", "\\x.x x");
    test_parse_convention("x x x x;", "x x x x");
    test_parse_convention("x (x x) x;", "x (x x) x");
    test_parse_convention("(\\x.x) (\\x.y);", "(\\x.x) \\x.y") ;
    test_parse_convention("\\x.a b c;", "\\x.a b c");
    test_parse_convention(
        "\\x.y (\\x.x) (\\x.y);",
        "\\x.y (\\x.x) (\\x.y)"
    );
    test_parse_convention(
        "\\x.y (\\x.x) (\\x.y) (\\x.y);",
        "\\x.y (\\x.x) (\\x.y) (\\x.y)"
    );
    test_parse_convention("(\\x.x x x x);", "\\x.x x x x");
    test_parse_convention(
        "(\\x.y (\\x.x) ((\\x.y) (\\x.y)));",
        "\\x.y (\\x.x) ((\\x.y) \\x.y)"
    );
    test_parse_convention("\\x.y (\\z.x);", "\\x.y (\\z.x)");


    puts("eval more paren");
    test_eval_all_paren("x;", "x");
    test_eval_all_paren("x x;", "(x x)");
    test_eval_all_paren("x (y z);", "(x (y z))");
    test_eval_all_paren("(\\x.x x);", "(\\x.(x x))");
    test_eval_all_paren("x x x x;", "(((x x) x) x)");
    test_eval_all_paren("x (x x) x;", "((x (x x)) x)");
    test_eval_all_paren("(\\x.x) (\\x.y);", "(\\x.y)");
    test_eval_all_paren("(\\x.x) (\\y.y) z;", "z");
    test_eval_all_paren("x ((\\x.x) (\\y.y));", "(x (\\y.y))");

    puts("eval less paren");
    test_eval_convention("x;", "x");
    test_eval_convention("x x;", "x x");
    test_eval_convention("x (y z);", "x (y z)");
    test_eval_convention("x x x x;", "x x x x");
    test_eval_convention("x (x x) x;", "x (x x) x");
    test_eval_convention("(\\x.x x);", "\\x.x x");
    test_eval_convention("(\\x.x) (\\x.y);", "\\x.y");
    test_eval_convention("(\\x.x) (\\y.y) z;", "z");
    test_eval_convention("x ((\\x.x) (\\y.y));", "x (\\y.y)");
    test_eval_convention( "(\\z.x z) ((\\x.x) (\\y.y));", "x (\\y.y)");
    test_eval_convention( "(\\x.x t) ((\\y.y) (\\z.z));", "t");
    test_eval_convention( "(\\z.z x) ((\\x.x) (\\y.y));", "x");
    test_eval_convention( "(\\z.\\x. z) y;", "\\x.y");
    test_eval_convention( "(\\z.\\x. z) x;", "\\#.x");
}
