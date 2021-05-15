// vim:filetype=c
#define  _GNU_SOURCE // for readline
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

// shortcuts
#define u64 uint64_t
#define u32 uint32_t
#define u8  uint8_t

#include "table.h"

#define ones(x)     (((u64)1) << (x))
#define mask(x, y)  ones((x) + (y) * 8)
#define clz(x)      __builtin_clzl(x)
#define ctz(x)      __builtin_ctzl(x)
#define popcount(x) __builtin_popcountl(x)

#define list_val_t void*

enum Color {
	WHITE = 0,
	BLACK = 1
};

enum Promo {
	PROMO_KNIGHT = 0,
	PROMO_QUEEN  = 1
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
	//u64 attacks[2]; ill probably need this for castling
	u64 occ[2]; // 0->white, 1->black
	u32 color : 1;
	//u32 castling  : 4;
	//u32 enPassant : 4;
} Board;

typedef struct Move {
	// When I use u32 instead of u8 Move becomes 4 bytes in size instead of 2
	// idk why TODO
	u8 src : 6; // 0 - 63
	u8 dst : 6; // 0 - 63
	u8 promo : 1; // 0 - 1
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

// === function declarations ===

// main.c
int main(void);

// board.c
void loadFen   (Board* b, const char* fen);
u32  pieceOn   (Board* b, int pos, u32 color);
void applyMove (Board* b, Board* old, Move* m); // b is the new board
void refreshOcc(Board* b);

// movegen.c
u64 genRayBB(u64 occ, u8 dir, u8 pos);
u64 genBMovesBB(u8 sq, Board* b);
u64 genRMovesBB(u8 sq, Board* b);
u64 genQMovesBB(u8 sq, Board* b);
u64 genKMovesBB(u8 sq, Board* b);
u64 genNMovesBB(u8 sq, Board* b);
u64 genPMovesBB(u8 sq, Board* b);
void bb2moves(u8 src, u64 bb, Board* b, List* l);
void genLegalMoves(Board* b, List* l);
bool leavesInCheck(Board* old, Move* m);

// util.c
u32  char2piece(char c);
u64  getMy(Board* b, int plr);
u8   bitScan(u64 bb, bool reverse);
int  max(int a, int b);
void readline(char* line);

// print.c
void printboard(Board* b, u64 hl);
void bb2char(u64 bb, char board[8][8], char c);
void bbprint(u64 bb);

// move.c
Move* moveFromString(char* string);
bool checkMoveString(char* string);

// eval.c
int eval(Board* b, u32 moveCount);
int evalCenter(Board* b);
int pieceDiff(Board* b, int piece);
int evalPieces(Board* b);
int evalMoves(u32 moveCount);

// search.c
int minimax(Board* b, int d);
Move* makeAIMove(Board* b, int d);

// debug.c
#ifdef DEBUG
void* dmalloc(size_t sz);
void  dfree(void* mem);
void  checkatend(void);
#else
#  define dmalloc malloc
#  define dfree free
#endif

// list.c
List* linit  (unsigned int num, list_val_t val[num]);
void  lfree  (List* l);
void  lappend(List* l, list_val_t val);
void  lpop   (List* l);

// === end of function declarations ===

