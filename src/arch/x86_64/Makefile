# @brief:  Makefile for building the compiler.
# @author: Ruturaj A. Nanoti

CC=gcc
CFLAGS=-g -Wall -Werror -Wextra -pedantic

#==============================================================================

SRCS=$(wildcard ./*.c)
INCS=-I ../inc -I ../inc/arch -I ../inc/arch/x86_64

OBJS=$(SRCS:.c=.o)

#==============================================================================

BUILD_DIR=../../../build
BIN_DIR=../../../bin

#==============================================================================

%.o:./%.c compile_msg
	$(CC) -c $(CFLAGS) $(INCS) $< -o ./$(BUILD_DIR)/$@

#==============================================================================

.PHONY: all clean build_and_bin_dir compile_msg run doxygen help clean_doxygen test

all: $(SRCS) $(BUILD_DIR) $(BIN_DIR) $(OBJS)

compile_msg:
	@printf "\033[1;34m[+] Compiling into object files ...\033[1;37m\n"
