int minimax(Board* b, int d) {
	List* l = linit(1, (void*[]){0});
	genLegalMoves(b, l);
	lpop(l);
	if (d == 0) {
		lfree(l);
		return eval(b, l->count);
	}
	int maxScore = checkmateScore;
	ListNode* pos = l->first;
	for (unsigned int i = 0; i < l->count; i++) {
		Board* newB = applyMove(b, (Move*)pos->val);
		int score = -minimax(newB, d - 1);
		dfree(newB);
		maxScore = max(maxScore, score);
		pos = pos->next;
	}
	lfree(l); // TODO udělat tohle při iterování
	return maxScore;
}

Move* makeAIMove(Board* b, int d) {
	int maxScore = checkmateScore;
	Move* bestMove = dmalloc(sizeof(Move));
	List* l = linit(1, (void*[]){0});
	genLegalMoves(b, l);
	lpop(l);
	ListNode* pos = l->first;
	for (unsigned int i = 0; i < l->count; i++) {
		Board* newB = applyMove(b, (Move*)pos->val);
		int score = -minimax(newB, d - 1);
		dfree(newB);
		if (score > maxScore) {
			maxScore = score;
			memcpy(bestMove, pos->val, sizeof(Move));
		}
		pos = pos->next;
	}
	lfree(l); // TODO udělat tohle při iterování
	return bestMove;
}
