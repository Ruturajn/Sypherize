# @brief:  Makefile for building the compiler.
# @author: Ruturaj A. Nanoti

CC=gcc
CFLAGS=-g -Wall -Werror
TARGET=sypherize

SRCS=$(wildcard ./src/*.c)
INCS=./inc

OBJS=$(notdir $(SRCS:.c=.o))

BUILD_DIR=build
BIN_DIR=bin

%.o:./src/%.c compile_msg
	$(CC) -c $(CFLAGS) $< -o ./$(BUILD_DIR)/$@

.PHONY: all clean build_and_bin_dir compile_msg

all: build_and_bin_dir $(TARGET)

build_and_bin_dir:
	@echo "\033[1;35m[ * ] Creating build and bin directory ...\033[1;37m"
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)

compile_msg:
	@echo "\033[1;33m[ * ] Compiling into object files ...\033[1;37m"

$(TARGET): $(SRCS) $(BUILD_DIR) $(BIN_DIR) $(OBJS)
	@echo "\033[1;33m[ * ] Linking into executable ...\033[1;37m"
	$(CC) $(CFLAGS) $(wildcard ./$(BUILD_DIR)/*.o) -I $(INCS) -o ./$(BIN_DIR)/$(TARGET)

clean:
	@echo "\033[1;33m[ * ] Cleaning build files ...\033[1;37m"
	rm -rf $(BUILD_DIR) $(BIN_DIR)
