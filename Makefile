# @brief:  Makefile for building the compiler.
# @author: Ruturaj A. Nanoti

CC=gcc
CFLAGS=-g -Wall -Werror
TARGET=clomp

SRCS=$(wildcard ./src/*.c)
INCS=./inc

OBJS=$(notdir $(SRCS:.c=.o))

BUILD_DIR=build
BIN_DIR=bin

%.o:./src/%.c
	$(CC) -c $(CFLAGS) $< -o ./$(BUILD_DIR)/$@

.PHONY: all clean

all: $(TARGET) $(BUILD_DIR) $(BIN_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(SRCS) $(BUILD_DIR) $(BIN_DIR) $(OBJS)
	$(CC) $(CFLAGS) $(wildcard ./$(BUILD_DIR)/*.o) -I $(INCS) -o ./$(BIN_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
