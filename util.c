#include "util.h"

#include "list.h"
#include "movegen.h"
#include "board.h"

extern Move* emptyMoveList[]; // from main.c

int max(int a, int b) { return a > b ? a : b; }
int min(int a, int b) { return a < b ? a : b; }

u32 char2piece(char c) {
	if (c == 'p' || c == 'P') return PAWN;
	if (c == 'r' || c == 'R') return ROOK;
	if (c == 'n' || c == 'N') return KNIGHT;
	if (c == 'b' || c == 'B') return BISHOP;
	if (c == 'k' || c == 'K') return KING;
	if (c == 'q' || c == 'Q') return QUEEN;
	printf("%s - unknown piece \"%d\"\n", __func__, c);
	exit(1);
}

u64 getMy(Board* b, int plr) {
	u64 my = (u64)0;
	for (int i = 0; i < 6; i++)
		my |= b->pieces[plr][i];
	return my;
}

// https://www.chessprogramming.org/BitScan#Generalized_Bitscan
u8 bitScan(u64 bb, bool reverse) {
	u64 rMask = -(u64)reverse;
	bb &= -bb | rMask;
	return (u8)(63 - clz(bb));
}

bool lcontains(List* l, Move* m) {
	ListNode* pos = l->first;
	for (u32 i = 0; i < l->count; i++) {
		//printf("move: %d -> %d\n", pos->m.src, pos->m.dst);
		if (pos->m.src == m->src && pos->m.dst == m->dst)
			return true;
		pos = pos->next;
	}
	return false;
}

u64 moveEnds(List* l, int i) {
	u64 mask = 0;
	ListNode* pos = l->first;
	while (pos != NULL) {
		if (pos->m.src == i) mask |= ones(pos->m.dst);
		pos = pos->next;
	}
	return mask;
}

void bb2char(u64 bb, char board[8][8], char c) {
	for (int i = ctz(bb); i < (64 - clz(bb)); i++)
		if (bb & ones(i))
			board[i / 8][i % 8] = c;
}

#ifdef DEBUG
unsigned int mallocs = 0;
unsigned int frees = 0;
size_t total = 0;

void* dmalloc(size_t sz) {
	void* mem = malloc(sz);
	total += sz;
	mallocs++;
	return mem;
}
void dfree(void* mem) {
	frees++;
	free(mem);
}
void checkatend() {
	printf("frees:   %d\nmallocs: %d\ntotal:   %ld bytes\n", frees, mallocs, total);
}
#endif

