CC = gcc
CFLAGS = -Wall -Wextra
OBJ = main.o resource.o
OUT = clite

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) -o $@ $^

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

resource.o: resource.rc
	windres $< -o $@

clean:
	del $(OBJ) $(OUT)
