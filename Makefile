CC = gcc
CFLAGS = -Wall -Wextra
OBJ = main.o resource.o syntax.o
OUT = clite.exe

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) -o $@ $^

main.o: main.c syntax.h 
	$(CC) $(CFLAGS) -c $< -o $@

syntax.o: syntax.c syntax.h
	$(CC) $(CFLAGS) -c $< -o $@

resource.o: resource.rc
	windres $< -o $@

clean:
	del $(OBJ) $(OUT)
