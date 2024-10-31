export TEMP := ./.git/info/tmp
export TMP := ./.git/info/tmp

CC = gcc
CFLAGS = -O3 -fopenmp
NAME = matmul
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj
LIB_DIR = lib

LIB_DIRS = $(shell find $(LIB_DIR) -type d -name "*link")
LIBS = $(patsubst %, -L./%, $(LIB_DIRS)) -lglfw3dll
INCLUDE_DIRS = $(shell find $(LIB_DIR) -type d -name "*include")
INCLUDES = $(patsubst %, -I./%, $(INCLUDE_DIRS))
SRCS = $(shell find $(SRC_DIR) $(LIB_DIR) -name "*.c")
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(OBJS) | $(BIN_DIR)
	@$(CC) $(CFLAGS) $(OBJS) $(INCLUDES) $(LIBS) -o $(BIN_DIR)/$(NAME)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)/$(NAME)

.PHONY: clean