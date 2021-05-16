int search(Board* s, int a, int b, int d) {
	List* l = linit(1, (void*[]){0});
	genLegalMoves(s, l);
	lpop(l);
	if (d == 0 || l->count == 0) {
		u32 count = l->count;
		lfree(l);
		return eval(s, count);
	}
	int score = -10000;
	ListNode* pos = l->first;
	while (1) {
		if (pos == NULL) break;
		Board newB;
		applyMove(&newB, s, (Move*)pos->val);
		score = -search(&newB, a, b, d - 1);
		if (score >= b) return b;
		a = max(a, score);
		pos = pos->next;
	}
	lfree(l); // TODO udělat tohle při iterování
	return a;
}

Move* makeAIMove(Board* b, int d) {
	int maxScore = checkmateScore;
	Move* bestMove = dmalloc(sizeof(Move));
	List* l = linit(1, (void*[]){0});
	genLegalMoves(b, l);
	lpop(l);
	ListNode* pos = l->first;
	for (unsigned int i = 0; i < l->count; i++) {
		Board newB;
		applyMove(&newB, b, (Move*)pos->val);
		int score = -search(&newB, -10000, 10000, d - 1);
		if (score > maxScore) {
			maxScore = score;
			memcpy(bestMove, pos->val, sizeof(Move)); // need to copy the data because it is gonna be freed by lfree
		}
		pos = pos->next;
	}
	lfree(l); // TODO udělat tohle při iterování
	return bestMove;
}

