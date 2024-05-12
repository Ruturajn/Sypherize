# @brief:  Makefile for building the compiler.
# @author: Ruturaj A. Nanoti

CC=g++
CFLAGS=-g -Wall -Werror -Wextra -pedantic

#==============================================================================

SRCS=$(wildcard ./*.cpp)
INCS=-I ../inc

OBJS=$(SRCS:.cpp=.o)

#==============================================================================

BUILD_DIR=../build
BIN_DIR=../bin

#==============================================================================

%.o:./%.cpp compile_msg
	$(CC) -c $(CFLAGS) $(INCS) $< -o ./$(BUILD_DIR)/$@

#==============================================================================

.PHONY: all compile_msg

all: $(SRCS) $(BUILD_DIR) $(BIN_DIR) $(OBJS)

compile_msg:
	@printf "\033[1;34m[+] Compiling into object files ...\033[1;37m\n"
