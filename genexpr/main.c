#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <lam.h>

typedef enum { Exptag, Apptag, Atomtag } Termtag;

void print_rand_expr(Termtag t) {
    bool subcase = rand() % 2 == 0;
    switch (t) {
        case Exptag: {
            if (subcase && rand() % 2 == 0) {
                print_rand_expr(Apptag);
            } else {
                printf("\\%c.", 's' + rand() % 8);
                print_rand_expr(Exptag);
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
                printf(" %c ", 's' + rand() % 8);
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
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    unsigned seed = spec.tv_sec * 1000000000 + spec.tv_nsec;
    srand(seed);
    print_rand_expr(Exptag);
    puts("");
}
