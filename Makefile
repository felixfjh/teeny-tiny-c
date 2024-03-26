CC = clang

CFLAGS += -Wall
CFLAGS += -Werror
CFLAGS += -Wextra

SRC = src/*.c
INC = -Isrc/*.h

BIN = tiny
OUT = -o $(BIN)

all:
	$(CC) $(CFLAGS) $(SRC) $(INC) $(OUT)

clean:
	rm $(BIN)
