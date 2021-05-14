CC = gcc

SOURCES = main.c

GCC_WFLAGS = -Wall -Wextra -Wuninitialized -Waggregate-return -Wunreachable-code -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wformat=2 -Winit-self -Wno-misleading-indentation -Wno-format-nonliteral
GCC_FLAGS  = $(GCC_WFLAGS) -std=c17 -march=native -DDEBUG -O0 -g
GCC_LIB    = -lm

NAME = chess
BUILD_NAME = $(shell git rev-parse --short HEAD)
BUILD_TIME = $(shell date +'%H%M%S_%d%m%y')

GCC_FULL = $(GCC_INCLUDES) $(GCC_FLAGS) $(GCC_LIB) -DBUILD_NAME=\"$(BUILD_NAME)\"

.PHONY: build

build:
	$(CC) $(SOURCES) $(GCC_FULL) -o $(NAME)
	@echo Build succeeded

clean:
	rm $(NAME)
