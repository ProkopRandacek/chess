#include "eval.h"

#include "table.h"

int eval(struct board* b, int moveCount) {
	return evalPieces(b) + evalCenter(b) + evalMoves(moveCount);
}

int evalCenter(struct board* b) {
	return center_score * popcount(b->occ[b->color_to_move] & center);
}

int pieceDiff(struct board* b, int piece) {
	return popcount(b->pieces[b->color_to_move][piece]) - popcount(b->pieces[!b->color_to_move][piece]);
}

int evalPieces(struct board* b) {
	int score = 0;
	for (int p = 0; p < 5; p++)
		score += piece_score[p] * pieceDiff(b, p);
	return score;
}

int evalMoves(int moveCount) {
	if (moveCount == 0) return checkmate_score;
	return (int)moveCount * move_score;
}

