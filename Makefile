# CC=gcc
# CC=gcc -fanalyzer
# CC=zig cc 
# CC=clang

CFLAGS:=-g -Wall -Wextra -pedantic -Iinclude -Ibdwgc/include
STRICT_CFLAGS:= -Werror

BUILD=./build

GC:=bdwgc/extra/gc.c
GCOBJ:=$(BUILD)/gc.o

LAM_OBJDIR=build
LAM_SRCDIR=./src
LAM_INCLUDE=include
LAM_HEADERS=$(wildcard include/*.h)
LAM_SRCS=$(wildcard src/*.c)
LAM_OBJ=$(LAM_SRCS:src/%.c=$(LAM_OBJDIR)/%.o)


FLEX_SRC=$(BUILD)/lex.yy.c
PARSER_DIR=parser
PARSER_INCLUDE=$(PARSER_DIR)/include
FLEX_OBJ=$(BUILD)/lex.yy.o
BISON_SRC=$(BUILD)/parser.tab.c
BISON_OBJ=$(BUILD)/parser.tab.o
FLEXBISON_OBJ=$(FLEX_OBJ) $(BISON_OBJ)
PARSER_UTIL=$(PARSER_DIR)/parser-lam-reader.c



run-tests: $(BUILD)/utests $(BUILD)/itests
	$(BUILD)/utests
	$(BUILD)/itests


$(BUILD)/utests: utests.c $(BISON_SRC) $(LAM_OBJ) $(GCOBJ) $(FLEX_OBJ) $(PARSER_UTIL) 
	$(CC) $(STRICT_CFLAGS) $(CFLAGS) -Iutest.h  -I$(PARSER_INCLUDE) -o $@ $^ 

$(BUILD)/itests: itests.c $(BISON_SRC) $(LAM_OBJ) $(GCOBJ) $(FLEX_OBJ) $(PARSER_UTIL)
	$(CC) $(STRICT_CFLAGS) $(CFLAGS) -Iutest.h -I$(PARSER_INCLUDE) -o $@ $^ 

$(LAM_OBJDIR)/%.o: $(LAM_SRCDIR)/%.c $(LAM_HEADERS)
	$(CC) $(LAMF) $(STRICT_CFLAGS) $(CFLAGS) -c -o $@  $<

$(BUILD)/parser: $(FLEXBISON_OBJ) $(PARSER_UTIL) $(LAM_OBJ) $(GCOBJ)
	$(CC) $(CFLAGS) -I$(PARSER_INCLUDE) -o $@ $(PARSER_DIR)/parser.c $^ -lfl


$(GCOBJ):
	$(CC) -c -o $(BUILD)/gc.o bdwgc/extra/gc.c -I bdwgc/include/

$(BUILD)/lex.yy.c: $(PARSER_DIR)/lexer.l $(BUILD)/parser.tab.c 
	flex -o $@ $<

$(BISON_SRC): $(PARSER_DIR)/parser.y
	bison -t -d -o $@ $<

$(BISON_OBJ): $(BISON_SRC)
	$(CC) -c -o $@ $^ -I$(LAM_INCLUDE) -I$(PARSER_INCLUDE)

$(FLEX_OBJ): $(FLEX_SRC)
	$(CC) -c -o $@ $^ -I$(LAM_INCLUDE) -I$(PARSER_INCLUDE)


tags: $(LAM_HEADERS) $(LAM_SRCS) $(PARSER_DIR) utests.c itests.c
	universal-ctags -R .

clean:
	find ./build/ -type f -delete


