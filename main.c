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
	char inp[10] = {0};
	Board board;
	loadFen(&board, "rnbqkbnr/pppppppp/////PPPPPPPP/RNBQKBNR");
	board.color = WHITE;
	u64 hl = 0;

	while (1) {
		List* l = linit(1, (void*[]){0});
		genLegalMoves(&board, l);
		lpop(l);
		if (l->count == 0) {
			printf("CHECKMATE: %s lost\n", board.color ? "black" : "white");
			lfree(l);
			break;
		}
		printf("\npossible moves: %d\n", l->count);
		printf("score: %d\n", eval(&board, l->count));
getInput:
		printboard(&board, hl);
		readline(inp);
		inp[strlen(inp)-1] = 0; // delete that ugly newline
		printf("you entered: %s\n", inp);

		if (strcmp(inp, "q") == 0) {
			lfree(l);
			break;
		}
		else if (strlen(inp) == 0) {
			printf("input is zero\n");
			goto getInput;
		}
		else {
			//if (checkMoveString(inp)) {
			if (true) {
				Move* move = moveFromString(inp);
				if (!(lcontains(l, move))) {
					printf("illegal move\n");
					goto getInput;
				}
				hl = ones(move->src) | ones(move->dst);
				Board newBoard;
				applyMove(&newBoard, &board, move);
				board = newBoard; // and replace it with new one
				dfree(move);
				printf("calculating AI move\n");
				printboard(&board, hl);
				move = makeAIMove(&board, 4);
				applyMove(&newBoard, &board, move);
				hl = ones(move->src) | ones(move->dst);
				board = newBoard;
				dfree(move);
			} else {
				printf("input does not match the algebraic notation\n");
				goto getInput;
			}
		}
		lfree(l);
	}
#ifdef DEBUG
	checkatend();
#endif
	return 0;
}

