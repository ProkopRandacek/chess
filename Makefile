CC = gcc

SOURCES = $(wildcard *.c)

CC_WFLAGS = -Wall -Wextra -Wuninitialized -Wunreachable-code -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wcast-qual -Wswitch-default -Wswitch-enum -Wformat=2 -Winit-self -Wno-misleading-indentation -Wno-format-nonliteral -Wconversion
#CC_FFLAGS = -fshort-enums
CC_FLAGS  = $(CC_WFLAGS) $(CC_FFLAGS) -std=c11 -march=native -Ofast -g -pg

# shared raylib lives in /usr/local/include
CC_RAYLIB = -I/usr/share/include -D_DEFAULT_SOURCE -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
CC_LIB    = -lm

NAME = chess
BUILD_NAME = $(shell git rev-parse --short HEAD)
BUILD_TIME = $(shell date +'%H%M%S_%d%m%y')

CC_FULL = $(CC_INCLUDES) $(CC_FLAGS) $(CC_LIB) $(CC_RAYLIB) -DBUILD_NAME=\"$(BUILD_NAME)\"

.PHONY: build

build:
	$(CC) $(SOURCES) $(CC_FULL) -o $(NAME)
	@echo Build succeeded

clean:
	rm $(NAME)
