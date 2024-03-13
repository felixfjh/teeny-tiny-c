CC = clang

# CFLAGS += -Wall
# CFLAGS += -Werror
# CFLAGS += -Wextra

SRC = *.c
INC = -I*.h

BIN = tiny
OUT = -o $(BIN)

all:
	$(CC) $(CFLAGS) $(SRC) $(INC) $(OUT)  

clean:
	rm $(BIN)
