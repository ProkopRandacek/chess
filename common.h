// vim:filetype=c
#ifndef COMMON_H
#define COMMON_H
#define  _GNU_SOURCE // for readline
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// shortcuts
#define u64 uint64_t
#define u32 uint32_t
#define u8  uint8_t

#include "table.h" // table needs shortcuts

#define ones(x)     (((u64)1) << (x))
#define mask(x, y)  ones((x) + (y) * 8)
#define clz(x)      __builtin_clzl(x)
#define ctz(x)      __builtin_ctzl(x)
#define popcount(x) __builtin_popcountl(x)

#define list_val_t void*

enum Color {
	// I cant call those WHITE and BLACK since these names are already used by raylib :c
	UPPER = 0, // white
	LOWER = 1  // black
};

enum Piece {
	PAWN   = 0b000,
	KNIGHT = 0b001,
	BISHOP = 0b010,
	ROOK   = 0b011,
	QUEEN  = 0b100,
	KING   = 0b101,
	EMPTY  = 0b110,
	//idk  = 0b111,
};

typedef struct Board {
	u64 pieces[2][6];
	u64 occ[2];
	u32 color : 1;
} Board;

typedef struct Move {
	u8 src : 6; // 0 - 63
	u8 dst : 6; // 0 - 63
} Move;

typedef struct ListNode ListNode;
typedef struct ListNode {
	list_val_t val;
	ListNode* next;
} ListNode;

typedef struct List {
	unsigned int count;
	ListNode* first;
	ListNode* last; // for fast append
} List;
#endif

