# @brief:  Makefile for building the compiler.
# @author: Ruturaj A. Nanoti

CC=g++
CFLAGS=-g -Wall -Werror -Wextra -pedantic
TARGET=sypherc
INCS=-I ../inc

#==============================================================================

BUILD_DIR=build
BIN_DIR=bin
FILE_PATH=./tests/test1.sy

#==============================================================================

SRCS = $(wildcard src/*.cpp)
OBJS = $(addprefix ./$(BUILD_DIR)/,$(notdir $(SRCS:.cpp=.o)))

#==============================================================================

.PHONY: all clean build_and_bin_dir run test

all: build_and_bin_dir $(TARGET)

build_and_bin_dir:
	@printf "\033[1;35m[+] Creating build and bin directory ...\033[1;37m\n"
	mkdir -p $(BUILD_DIR) $(BIN_DIR)

compile_msg:
	@printf "\033[1;34m[+] Compiling into object files ...\033[1;37m\n"

$(TARGET):
	@$(MAKE) -C ./src
	@printf "\033[1;32m[+] Linking into executable ...\033[1;37m\n"
	$(CC) $(CFLAGS) $(OBJS) $(INCS) -o ./$(BIN_DIR)/$(TARGET)
	@printf "\033[1;36m[+] DONE\033[1;37m\n"

clean:
	@printf "\033[1;33m[+] Cleaning generated build files ...\033[1;37m\n"
	rm -rf $(BUILD_DIR) $(BIN_DIR)

run: clean all
	@printf "\033[1;33m[+] Running the executable ...\033[1;37m\n"
	./$(BIN_DIR)/$(TARGET) $(FILE_PATH)

test:
	@./$(BIN_DIR)/$(TARGET) $(FILE_PATH)
