//bool canCastle(Board* b, int pos) { return (b->castling & (1 << pos)); }
//void disableCastle(Board* b, int pos) { b->castling &= ~(1 << pos); }

void loadFen(Board* b, const char* fen) {
	memset(b->pieces, 0, 2 * 6 * sizeof(u64));
	int x = 0;
	int y = 0;
	for (int i = 0; i < (int)strlen(fen); i++) { // TODO strlen bad in this case
		if (fen[i] == '/') {
			y++;
			x = 0;
		} else {
			if (fen[i] >= '0' && fen[i] <= '9') {
				x += (char)(fen[i] - 48);
			} else {
				u32 c = (fen[i] >= 'A' && fen[i] <= 'Z') ? WHITE : BLACK;
				b->pieces[c][char2piece(fen[i])] |= mask(x, y);
				x++;
			}
		}
	}
	b->occ[0] = b->pieces[0][0] | b->pieces[0][1] | b->pieces[0][2] | b->pieces[0][3] | b->pieces[0][4] | b->pieces[0][5];
	b->occ[1] = b->pieces[1][0] | b->pieces[1][1] | b->pieces[1][2] | b->pieces[1][3] | b->pieces[1][4] | b->pieces[1][5];
}

u32 pieceOn(Board* b, int pos, u32 color) {
	for (u32 i = 0; i < 6; i++)
		if (b->pieces[color][i] & ones(pos))
			return i;
	printf("%s - could not find piece on x%d y%d (%d).\n", __func__, pos % 8, pos / 8, pos);
	exit(1);
}

void clearPos(Board* b, int pos) {
	u64 mask = ~ones(pos);
	for (int c = 0; c < 2; c++)
		for (int p = 0; p < 6; p++)
			b->pieces[c][p] &= mask;
}

//void refreshAttacks(Board* b, u32 c) { }

Board* applyMove(Board* old, Move* m) {
	u32 p = m->promo;
	Board* b = dmalloc(sizeof(Board));
	memcpy(b->pieces, old->pieces, 2 * 6 * sizeof(u64));
	//b->castling = old->castling;
	b->color = !old->color;

	if (m->promo == 0)
		p = pieceOn(old, m->src, old->color);

	// disabling castling moves
	/*if (p == KING) {
		if (m->src == 60) {
			disableCastle(b, 2);
			disableCastle(b, 3);
		} else if (m->src == 4) {
			disableCastle(b, 0);
			disableCastle(b, 1);
		}
	} else if (p == ROOK) {
		     if (m->src == 0 ) { disableCastle(b, 0); }
		else if (m->src == 7 ) { disableCastle(b, 1); }
		else if (m->src == 56) { disableCastle(b, 2); }
		else if (m->src == 63) { disableCastle(b, 3); }
	} else {
		     if (m->dst == 0 ) { disableCastle(b, 0); }
		else if (m->dst == 7 ) { disableCastle(b, 1); }
		else if (m->dst == 56) { disableCastle(b, 2); }
		else if (m->dst == 63) { disableCastle(b, 3); }
	}
	if (p == KING) {
		if (m->src == 4) {
			if (m->dst == 2) {
				b->pieces[b->color][ROOK] |= ones(3);
				clearPos(b, 0);
			} else if (m->dst == 6) {
				b->pieces[b->color][ROOK] |= ones(5);
				clearPos(b, 7);
			}
		} else if (m->src == 60) {
			if (m->dst == 58) {
				b->pieces[b->color][ROOK] |= ones(59);
				clearPos(b, 56);
			} else if (m->dst == 62) {
				b->pieces[b->color][ROOK] |= ones(61);
				clearPos(b, 63);
			}
		}
	}*/

	clearPos(b, m->src);
	clearPos(b, m->dst);
	b->pieces[!b->color][p] |= ones(m->dst);

	b->occ[0] = b->pieces[0][0] | b->pieces[0][1] | b->pieces[0][2] | b->pieces[0][3] | b->pieces[0][4] | b->pieces[0][5];
	b->occ[1] = b->pieces[1][0] | b->pieces[1][1] | b->pieces[1][2] | b->pieces[1][3] | b->pieces[1][4] | b->pieces[1][5];

	//refreshAttacks(b, BLACK);
	//refreshAttacks(b, WHITE);

	return b;
}
