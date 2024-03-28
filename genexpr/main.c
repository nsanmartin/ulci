#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <lam.h>

typedef enum { Exptag, Apptag, Atomtag } Termtag;

void print_rand_expr(Termtag t) {
    bool subcase = rand() % 2 == 0;
    switch (t) {
        case Exptag: {
            if (subcase) {
                printf("\\%s.", "x");
                print_rand_expr(Exptag);
            } else {
                print_rand_expr(Apptag);
            }
            break;
        }
        case Apptag: {
            if (subcase) {
                print_rand_expr(Atomtag);
            } else {
                print_rand_expr(Apptag);
                print_rand_expr(Atomtag);
            }
            break;
        }
        case Atomtag: {
            if (subcase) {
                printf(" %s ", "x");
            } else {
                printf("(");
                print_rand_expr(Exptag);
                printf(")");
            }
            break;
        }
        default: LOG_INVALID_LTERM_AND_EXIT;
    }
}

int main(int argc, char* argv[]) {
    srand(time(NULL)); // use current time as seed for random generator
    print_rand_expr(Exptag);
    puts("");
}
