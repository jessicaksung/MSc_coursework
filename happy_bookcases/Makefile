CC=gcc
CFLAGS = -Wall -Wextra -pedantic -Wfloat-equal -std=c90

COMMON=-Wall -Wextra -Wfloat-equal -pedantic -std=c90
DEBUG= -g3
SANITIZE= $(COMMON) -fsanitize=undefined -fsanitize=address $(DEBUG)
VALGRIND= $(COMMON) $(DEBUG)
PRODUCTION= $(COMMON) -O3
LDLIBS= -lm

all: bookcase_s bookcase_v bookcase_p

bookcase_s: bookcase.c
	$(CC) bookcase.c -o bookcase_s $(SANITIZE) $(LDLIBS)

bookcase_v: bookcase.c
	$(CC) bookcase.c -o bookcase_v $(VALGRIND) $(LDLIBS)

bookcase_p: bookcase.c
	$(CC) bookcase.c -o bookcase_p $(PRODUCTION)

extension_s: extension.c simple_screen/neillsimplescreen.c simple_screen/neillsimplescreen.h
	$(CC) extension.c simple_screen/neillsimplescreen.c -o extension_s $(SANITIZE) $(LDLIBS)

extension_v: extension.c simple_screen/neillsimplescreen.c simple_screen/neillsimplescreen.h
	$(CC) extension.c simple_screen/neillsimplescreen.c -o extension_v $(VALGRIND) $(LDLIBS)

extension_p: extension.c simple_screen/neillsimplescreen.c simple_screen/neillsimplescreen.h
	$(CC) extension.c simple_screen/neillsimplescreen.c -o extension_p $(PRODUCTION)

run: bookcase_v
	valgrind --leak-check=full ./bookcase_v yyybbb-334.bc verbose
