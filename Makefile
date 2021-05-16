CC = gcc

SOURCES = main.c

CC_WFLAGS = -Wall -Wextra -Wuninitialized -Waggregate-return -Wunreachable-code -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wformat=2 -Winit-self -Wno-misleading-indentation -Wno-format-nonliteral
CC_FLAGS  = $(CC_WFLAGS) -std=c17 -march=native -Ofast -g
CC_LIB    = -lm

NAME = chess
BUILD_NAME = $(shell git rev-parse --short HEAD)
BUILD_TIME = $(shell date +'%H%M%S_%d%m%y')

CC_FULL = $(CC_INCLUDES) $(CC_FLAGS) $(CC_LIB) -DBUILD_NAME=\"$(BUILD_NAME)\"

.PHONY: build

build:
	$(CC) $(SOURCES) $(CC_FULL) -o $(NAME)
	@echo Build succeeded

clean:
	rm $(NAME)
