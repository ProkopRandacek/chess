#include "util.h"

#include "list.h"
#include "movegen.h"
#include "board.h"

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

u8 bitScan(u64 bb, bool reverse) {
	if (reverse) return (u8)(63 - clz(bb));
	return (u8)ctz(bb);
}

bool lcontains(List* l, Move* m) {
	ListNode* pos = l->first;
	for (u32 i = 0; i < l->count; i++) {
		printf("move: %d -> %d\n", ((Move*)(pos->val))->src, ((Move*)(pos->val))->dst);
		if (((Move*)(pos->val))->src == m->src && ((Move*)(pos->val))->dst == m->dst)
			return true;
		pos = pos->next;
	}
	return false;
}

void readline(char* line) {
	size_t size;
	if (getline(&line, &size, stdin) == -1) printf("No line\n");
}

int moveGenTest(Board* b, int d) {
	if (d == 0) return 1;

	List* l = linit(1, (void*[]){0});
	genLegalMoves(b, l);
	lpop(l);
	int numPositions = 0;

	ListNode* pos = l->first;
	while (1) {
		if (pos == NULL) break;
		Board newB;
		applyMove(&newB, b, (Move*)pos->val);

		numPositions += moveGenTest(&newB, d - 1);

		pos = pos->next;
	}
	return numPositions;
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

