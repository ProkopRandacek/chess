#include "common.h"
#include "board.c" // is this evil?
#include "debug.c" // works nice tho
#include "list.c"
#include "movegen.c"
#include "util.c"
#include "print.c"
#include "move.c"
#include "eval.c"
#include "search.c"

int main(void) {
	printf("\n\n=== start ===\n\n");

	char inp[10] = {0};
	Board* board = dmalloc(sizeof(Board));
	loadFen(board, "rnbqkbnr/pppppppp/////PPPPPPPP/RNBQKBNR");
	board->color = WHITE;
	u64 hl = 0;

	while (1) {
		List* l = linit(1, (void*[]){0});
		genLegalMoves(board, l);
		lpop(l);
		if (l->count == 0) {
			printf("CHECKMATE: %s lost\n", board->color ? "black" : "white");
			lfree(l);
			break;
		}
		printf("\npossible moves: %d\n", l->count);
		printf("score: %d\n", eval(board, l->count));
		printboard(board, hl);
getInput:
		readline(inp);
		inp[strlen(inp)-1] = 0; // delete that ugly newline

		if (strcmp(inp, "q") == 0) {
			lfree(l);
			break;
		}
		//else if (strcmp(inp, "i") == 0) printinfo(board);
		else if (strlen(inp) == 0) goto getInput;
		else {
			//if (checkMoveString(inp)) {
			if (true) {
				Move* move = moveFromString(inp);
				if (!(lcontains(l, move))) {
					printf("illegal move\n");
					goto getInput;
				}
				hl = ones(move->src) | ones(move->dst);
				Board* newBoard = applyMove(board, move);
				dfree(board); // free old board
				board = newBoard; // and replace it with new one
				dfree(move);

				Move* aimove = aiMove(board);
				board = applyMove(board, aimove);
				dfree(aimove);
			} else {
				printf("input does not match the algebraic notation\n");
				goto getInput;
			}
		}
		lfree(l);
	}
	dfree(board);
#ifdef DEBUG
	checkatend();
#endif
	printf("\n\n=== end ===\n");
	return 0;
}

Move* aiMove(Board* b) {
	printf("AI is thinking...\n");
	return makeAIMove(b, 4);
}
