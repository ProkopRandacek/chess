static const int pieceScore[5] = {
	10, // pawn
	30, // knight
	30, // bishop
	50, // rook
	90  // queen
};
static const int checkmateScore = -10000;
static const int centerScore = 1;
static const int moveScore = 1;
static const u64 center = 0;

int eval(Board* b, u32 moveCount) {
	return evalPieces(b) + evalCenter(b) + evalMoves(moveCount);
}

int evalCenter(Board* b) {
	return centerScore * popcount(b->occ[b->color] & center);
}

int pieceDiff(Board* b, int piece) {
	return popcount(b->pieces[b->color][piece]) - popcount(b->pieces[!b->color][piece]);
}

int evalPieces(Board* b) {
	int score = 0;
	for (int p = 0; p < 5; p++)
		score += pieceScore[p] * pieceDiff(b, p);
	return score;
}

int evalMoves(u32 moveCount) {
	if (moveCount == 0) return checkmateScore;
	return (int)moveCount * moveScore;
}

