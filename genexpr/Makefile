CFLAGS:=-g  -Wall -Wextra -pedantic -Iinclude
STRICT_CFLAGS:= -Werror

main:
	$(CC) $(STRICT_CFLAGS) $(CFLAGS) -I../include -o build/$@ $@.c


clean:
	find build -type f -delete
