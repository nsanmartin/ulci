#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <lam.h>

typedef enum { Exptag, Apptag, Atomtag } Termtag;

enum { IntArrInitLen = 800 };

typedef struct {
    int* buf;
    size_t len;
    size_t capacity;
} IntArray;

void int_arr_init(IntArray arr[static 1], size_t capacity) {
    int* buf = malloc(IntArrInitLen * sizeof(int));
    if (buf == NULL) {
        puts("Memory error (malloc), aborting");
        exit(EXIT_FAILURE);
    }
    *arr = (IntArray) { .buf=buf, .len=0, .capacity=capacity };
}

void free_int_array(IntArray arr[static 1]) {
    if (arr) {
        free (arr->buf);
    }
}


bool int_arr_is_full(IntArray arr[static 1]) { return arr->len >= arr->capacity; }

int int_arr_push(IntArray arr[static 1], int x) {
    if (!arr || !arr->buf) { return 0; }
    if (int_arr_is_full(arr)) {
        size_t new_capacity = 2 * arr->capacity;
        arr->buf = realloc(arr->buf, new_capacity * sizeof(int));
        arr->capacity = new_capacity;
        if (arr->buf == NULL) {
            puts("Memory error (realloc), aborting");
            exit(EXIT_FAILURE);
        }
    }
    arr->buf[arr->len++] = x;
    return 0;
}

IntArray bool_generator = {0};
IntArray char_generator = {0};

void init_dynamic_arrays() {
    int_arr_init(&bool_generator, IntArrInitLen);
    int_arr_init(&char_generator, IntArrInitLen);
}

void int_arr_bin_print(IntArray arr[static 1]) {
    for (size_t i = 0; i < arr->len; ++i) {
        printf("%d", arr->buf[i]);
        if (i && (i+1) % 4 == 0) { printf(" "); }
    }
    puts("");
}

void int_arr_char_print(IntArray arr[static 1]) {
    for (size_t i = 0; i < arr->len; ++i) {
        printf("%c", arr->buf[i]);
    }
    puts("");
}

void int_arr_print(IntArray arr[static 1]) {
    for (size_t i = 0; i < arr->len; ++i) {
        printf("%d ", arr->buf[i]);
    }
    puts("");
}

void free_dynamic_arrays(void) {
    free_int_array(&bool_generator);
    free_int_array(&char_generator);
}


/// BitSource
typedef struct {
    const unsigned* buf;
    size_t sz;
    size_t next;
} BitSource;

const size_t _unsigned_len = 8 * sizeof(unsigned);

bool bit_source_exhausted(BitSource bs[static 1]) {
    return bs->next >= bs->sz * _unsigned_len;
}


/// Char Source
typedef struct {
    const char* buf;
    size_t sz;
    size_t next;
} CharSource;


bool char_source_exhausted(CharSource cs[static 1]) { return cs->next >= cs->sz; }


/// Random sources
int bool_from_rand() {
    int res = rand() % 2;
    int_arr_push(&bool_generator, res);
    return res;
}

int char_from_rand() {
    int res = 's' + rand() % 8;
    int_arr_push(&char_generator, res);
    return res;
}


int bool_from_source(BitSource bs[static 1]) {
    if (!bs->buf || bit_source_exhausted(bs)) { puts("Not enough bit to generate expresion, aborting"); exit(EXIT_FAILURE); }
    size_t next = bs->next++;
    size_t index = next / _unsigned_len;
    size_t offset = next % _unsigned_len;
    unsigned num = bs->buf[index];
    int res = (num >> offset) & 0x1;
    return res;
}

int char_from_source(CharSource cs[static 1]) {
    if (!cs->buf || char_source_exhausted(cs)) { puts("Not enough chars to generate expresion, aborting"); exit(EXIT_FAILURE); }
    return cs->buf[cs->next++];
}

typedef struct { BitSource* bs; CharSource* cs; } DataSource;

int next_bool(DataSource* ds) {
    if (ds && ds->bs) {
        return bool_from_source(ds->bs);
    } else {
        return bool_from_rand();
    }
}

int next_char(DataSource* ds) {
    if (ds && ds->cs) {
        return char_from_source(ds->cs);
    } else {
        return char_from_rand();
    }
}


void print_rand_expr(Termtag t, DataSource* ds) {
    bool subcase = next_bool(ds) == 0;
    switch (t) {
        case Exptag: {
            if (subcase && next_bool(ds) == 0) {
                print_rand_expr(Apptag, ds);
            } else {
                printf("\\%c.", next_char(ds));
                print_rand_expr(Exptag, ds);
            }
            break;
        }
        case Apptag: {
            if (subcase) {
                print_rand_expr(Atomtag, ds);
            } else {
                print_rand_expr(Apptag, ds);
                print_rand_expr(Atomtag, ds);
            }
            break;
        }
        case Atomtag: {
            if (subcase) {
                printf(" %c ", next_char(ds));
            } else {
                printf("(");
                print_rand_expr(Exptag, ds);
                printf(")");
            }
            break;
        }
        default: LOG_INVALID_LTERM_AND_EXIT;
    }
}


#define ArrLen(A) (sizeof(A)/sizeof(*A))
#define makeDataSourceFromArrs(BS, CS)                                                                                       \
    (DataSource) {.bs=&(BitSource){.buf=BS, .sz=ArrLen(BS)}, .cs=&(CharSource){.buf=CS, .sz=ArrLen(CS)}}


void generate_rand_expression_logging(unsigned int seed) {
    srand(seed);
    init_dynamic_arrays();

    //DataSource ds = makeDataSourceFromArrs( ((unsigned[]){0xaf1230, 12376123, 98734}), "xyzwxzstyxzzzzzzzzzzzzzzzz");
    print_rand_expr(Exptag, NULL);
    puts("");
    int_arr_bin_print(&bool_generator);
    int_arr_char_print(&char_generator);

    free_dynamic_arrays();
}

void generate_rand_expression(unsigned int seed) {
    srand(seed);

    //DataSource ds = makeDataSourceFromArrs( ((unsigned[]){0xaf1230, 12376123, 98734}), "xyzwxzstyxzzzzzzzzzzzzzzzz");
    print_rand_expr(Exptag, NULL);
    puts("");
}

int main(int argc, char* argv[]) {
    if (argc == 0) {
        struct timespec spec;
        clock_gettime(CLOCK_REALTIME, &spec);
        unsigned seed = spec.tv_sec * 1000000000 + spec.tv_nsec;
        generate_rand_expression(seed);
    }
    for (int i = 1; i < argc; ++i) {
        unsigned long seed = strtoul(argv[i], NULL, 10);
        if (!seed) { printf("Invalod input %s\n", argv[i]); }
        else { generate_rand_expression(seed); }
    }
}

