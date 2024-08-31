CC = gcc
CFLAGS = -Wall -Wextra
OBJ = main.o resource.o syntax.o console_utils.o logger.o
OUT = clite.exe

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) -o $@ $^

main.o: main.c syntax.h console_utils.h logger.h
	$(CC) $(CFLAGS) -c $< -o $@

syntax.o: syntax.c syntax.h
	$(CC) $(CFLAGS) -c $< -o $@

console_utils.o: console_utils.c console_utils.h
	$(CC) $(CFLAGS) -c $< -o $@

logger.o: logger.c logger.h
	$(CC) $(CFLAGS) -c $< -o $@

resource.o: resource.rc
	windres $< -o $@

clean:
	del $(OBJ) $(OUT)
