// #include "parser.tab.c"
#include <stdio.h>

#include "parser-util.h"

void test_parse_impl(const char* s, const char* r) {
    printf("parsing '%s' => %s\n", s, r);
    if (strcmp(s,r)) {
        fprintf(stderr, "Error parsing %s\n", s);
    } else {
        printf("Ok\n");
    }
}

#define test_parse(S)                                                   \
    test_parse_impl(S, parse_string( S "\n").s);

int main (void) {
    test_parse("x");
    test_parse("(x x)");
    test_parse("(\\x.(x x))");
    test_parse("(\\x.(x x))");
    test_parse("((\\x.xx) (\\x.xy))");
}
