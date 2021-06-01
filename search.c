#include "search.h"

#include "board.h"
#include "debug.h"
#include "eval.h"
#include "list.h"
#include "movegen.h"
#include "util.h"

extern Move* emptyMoveList[]; // from main.c

int search(Board* s, int a, int b, int d) {
	List* l = linit(1, emptyMoveList);
	genLegalMoves(s, l);
	lpop(l);
	if (d == 0 || l->count == 0) {
		u32 count = l->count;
		lfree(l);
		return eval(s, count);
	}

	int score = checkmateScore;
	ListNode* pos = l->first;
	while (pos != NULL) {
		{
			Board newB;
			applyMove(&newB, s, &pos->m);
			score = -search(&newB, a, b, d - 1);
		}
		if (score >= b) return b;
		a = max(a, score);
		ListNode* oldpos = pos;
		pos = pos->next;
		lreturnnode(oldpos);
	}
	dfree(l);
	return a;
}

Move* makeAIMove(Move* bestMove, Board* b, int d) {
	List* l = linit(1, emptyMoveList);
	genLegalMoves(b, l);
	lpop(l);

	int maxScore = checkmateScore;
	int score;
	ListNode* pos = l->first;
	while (pos != NULL) {
		{
			Board newB;
			applyMove(&newB, b, &pos->m);
			score = -search(&newB, -10000, 10000, d - 1);
		}
		if (score > maxScore) {
			maxScore = score;
			memcpy(bestMove, &pos->m, sizeof(Move)); // need to copy the data because it is gonna be freed
		}
		ListNode* oldpos = pos;
		pos = pos->next;
		lreturnnode(oldpos);
	}
	dfree(l);
	return bestMove;
}

