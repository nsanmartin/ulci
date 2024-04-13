// #include "parser.tab.c"
#include <stdio.h>

#include "parser-lam-reader.h"
#include <symbols.h>

int test_parse_impl(const char* s, const char* out, const char* expected) {
    if (strcmp(out, expected)) {
        fprintf(
            stderr,
            RED "itests: error parsing or reducing" RESET "\n"
            "  test\t %s\n  res\t %s\n  exp\t %s\n",
            s, out, expected);
        return -1;
    } else {
        printf("%s " GREEN "Ok" RESET "\n", s);
        return 0;
    }
}

void test_parse_all_paren(const char* test, const char* expected) {
    const char* obtained = parse_string_rec_desc_to_str(test, lam_term_to_str_more_paren).s;
    if(test_parse_impl(test, obtained, expected)) {
        exit(1);
    };
}

void test_parse_convention(const char* test, const char* expected) {
    const char* obtained = parse_string_rec_desc_to_str(test, lam_term_to_str_less_paren).s;
    if(test_parse_impl(test, obtained, expected)) {
        exit(1);
    };
}

void test_eval_all_paren(const char* test, const char* expected) {
    const char* obtained = parse_string_rec_desc_reduce_to_str(test, lam_term_to_str_more_paren).s;
    if(test_parse_impl(
        test, obtained, expected
    )) {
        exit(1);
    };
}

void test_eval_convention(const char* test, const char* expected) {
    const char* obtained = parse_string_rec_desc_reduce_to_str(test, lam_term_to_str_less_paren).s;
    if(test_parse_impl(
        test, obtained, expected
    )) {
        exit(1);
    };
}

int main (void) {
    if(initialize_symbol_table()) {
        fprintf(stderr, "Error: not memory to initialize parser\n");
        return -1;
    }
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
    test_parse_all_paren(
         "\\x.f x;", "(\\x.(f x))"
    );
    test_parse_all_paren(
         "f(\\x.f x);", "(f (\\x.(f x)))"
    );
    test_parse_all_paren(
         "\\f.(f(\\x.f x));", "(\\f.(f (\\x.(f x))))"
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

    puts("eval definitions");
    test_eval_convention( "set id = \\x.x", "\\x.x");
    test_eval_convention( "id y;", "y");
    // redefine id
    test_eval_convention( "set id2 = \\y.y;", "\\y.y");
    test_eval_convention( "id x x;", "x x");
    test_eval_convention( "id2 id2 y;", "y");
    test_eval_convention( "id2 (x y);", "x y");
    test_eval_convention(
        "set apply = \\f.f(\\x.(f x));", "\\f.f (\\x.f x)"
    );
    test_eval_convention( "apply id x;", "x");
    test_eval_convention(
        "set dup = \\f. f f; dup g;", "g g"
    );
    test_eval_convention(
        "set const = a_const_name;(\\x.x) const;",
        "a_const_name"
    );
    test_eval_convention("set true = \\x.\\y.x;", "\\x.\\y.x");
    test_eval_convention("set false = \\x.\\y.y;", "\\x.\\y.y");
    test_eval_convention(
        "set ifelse = \\p.\\a.\\b.p a b;", "\\p.\\a.\\b.p a b"
    );
    test_eval_convention("ifelse true a b;", "a");
    test_eval_convention("ifelse false a b;", "b");
    test_eval_convention("set zero = \\f.\\x.x;", "\\f.\\x.x");
    test_eval_convention(
        "set succ = \\n.\\f.\\x.f(n f x);",
       "\\n.\\f.\\x.f (n f x)"
    );
    test_eval_convention("set one = succ zero;", "\\f.\\x.f x");
    test_eval_convention("set two = succ one;", "\\f.\\x.f (f x)");
    test_eval_convention("set three = succ two;", "\\f.\\x.f (f (f x))");
    test_eval_convention("set four = succ three;", "\\f.\\x.f (f (f (f x)))");
    test_parse_convention(
        "set plus = \\m.\\n.\\f.\\x.m f (n f x);",
        "\\m.\\n.\\f.\\x.m f (n f x)"
    );

    test_eval_convention("plus zero zero;",  "\\f.\\x.x");
    test_eval_convention("plus one one;", "\\f.\\x.f (f x)");
    test_eval_convention("plus two one;",  "\\f.\\x.f (f (f x))");
    test_eval_convention("plus one two;",  "\\f.\\x.f (f (f x))");
    test_eval_convention("plus two two;", "\\f.\\x.f (f (f (f x)))");
    test_eval_convention("plus three one;", "\\f.\\x.f (f (f (f x)))");
    test_eval_convention("plus one three;", "\\f.\\x.f (f (f (f x)))");

    puts("eval subst");
    test_eval_convention("(\\x. x x) (\\x.x)",  "\\x.x");
    test_eval_convention("((\\x.(x y)) (\\x.(x y)))", "y y");


}

