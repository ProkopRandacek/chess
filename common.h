#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ucw/lib.h>

#define one(x)        (((uint64_t)1) << (x))
#define one_sq(x, y)  one((x) + (y)*8)
#define clz(x)        __builtin_clzl(x)
#define ctz(x)        __builtin_ctzl(x)
#define popcount(x)   __builtin_popcountl(x)

#define IS_UPPER(x) (((x) >= 'A') && ((x) <= 'Z'))
#define IS_LOWER(x) (((x) >= 'a') && ((x) <= 'z'))
#define IS_DIGIT(x) (((x) >= '0') && ((x) <= '9'))

//! Prints a formatted string.
#define log(fmt, ...) printf("%s:%d (%s) - " fmt "\n", __FILE__, __LINE__, __func__, ## __VA_ARGS__)

//! Prints a formatted string and dies.
#define err(fmt, ...) log(fmt, ## __VA_ARGS__),exit(1)

//! Die if false
#define assert(EXR) do { if (unlikely(!(EXR))) log("assertion failed (" #EXR ")"),exit(1); } while(0)

typedef uint64_t bb;

enum piece {
	pPAWN   = 0,
	pKNIGHT = 1,
	pBISHOP = 2,
	pROOK   = 3,
	pQUEEN  = 4,
	pKING   = 5,
	pNONE   = 6,
};

enum piece char2piece(char c);

extern char* piece_spelling[];

