# @brief:  Makefile for building the compiler.
# @author: Ruturaj A. Nanoti

CC=gcc
CFLAGS=-g -Wall -Werror
TARGET=sypherize

#==============================================================================

SRCS=$(wildcard ./src/*.c)
INCS=./inc

OBJS=$(notdir $(SRCS:.c=.o))

#==============================================================================

BUILD_DIR=build
BIN_DIR=bin
DOCS_DIR=docs
FILE_PATH=./example/simple.sy

#==============================================================================

%.o:./src/%.c compile_msg
	$(CC) -c $(CFLAGS) $< -o ./$(BUILD_DIR)/$@

#==============================================================================

.PHONY: all clean build_and_bin_dir compile_msg run docs help

all: build_and_bin_dir $(TARGET)

build_and_bin_dir:
	@echo "\033[1;35m[+] Creating build and bin directory ...\033[1;37m"
	mkdir -p $(BUILD_DIR) $(BIN_DIR)

compile_msg:
	@echo "\033[1;34m[+] Compiling into object files ...\033[1;37m"

$(TARGET): $(SRCS) $(BUILD_DIR) $(BIN_DIR) $(OBJS)
	@echo "\033[1;32m[+] Linking into executable ...\033[1;37m"
	$(CC) $(CFLAGS) $(addprefix ./$(BUILD_DIR)/,$(OBJS)) -I $(INCS) -o ./$(BIN_DIR)/$(TARGET)
	@echo "\033[1;36m[+] DONE\033[1;37m"

clean:
	@echo "\033[1;33m[+] Cleaning generated build files ...\033[1;37m"
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(DOCS_DIR)

docs:
	@echo "\033[1;34m[+] Generating Documentation ...\033[1;37m"
	rm -rf $(DOCS_DIR)
	mkdir -p $(DOCS_DIR)
	doxygen Doxyfile
	$(MAKE) -C ./docs/latex/

run: clean all
	@echo "\033[1;33m[+] Running the executable ...\033[1;37m"
	./$(BIN_DIR)/$(TARGET) $(FILE_PATH)

help:
	@echo ""
	@echo "\033[1;33mUSAGE:\033[1;37m"
	@echo "    make <TAGRET>"
	@echo ""
	@echo "\033[1;33mTARGETS:\033[1;37m"
	@echo "    \033[1;35mall\033[1;37m   - Complete build (Default Target)."
	@echo "    \033[1;35mclean\033[1;37m - Remove build files and directories."
	@echo "    \033[1;35mrun\033[1;37m   - Executes all, clean and runs the executable."
	@echo "            Optionally provide 'FILE_PATH' for running"
	@echo "            the executable."
	@echo "    \033[1;35mdocs\033[1;37m  - Generates documentation, using doxygen."
	@echo "    \033[1;35mhelp\033[1;37m  - Prints out this help menu."
