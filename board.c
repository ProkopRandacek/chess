#include "board.h"

#include "util.h"

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
				u32 c = (fen[i] >= 'A' && fen[i] <= 'Z') ? LOWER : UPPER;
				b->pieces[c][char2piece(fen[i])] |= mask(x, y);
				x++;
			}
		}
	}
	refreshOcc(b);
}

void refreshOcc(Board* b) {
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

int safePieceOn(Board* b, int pos) {
	for (int c = 0; c < 2; c++)
		for (u32 p = 0; p < 6; p++)
			if (b->pieces[c][p] & ones(pos))
				return c * 6 + p;
	return -1;
}

void clearPos(Board* b, int pos) {
	u64 mask = ~ones(pos);
	for (int c = 0; c < 2; c++)
		for (int p = 0; p < 6; p++)
			b->pieces[c][p] &= mask;
}

void applyMove(Board* b, Board* old, Move* m) {
	memcpy(b->pieces, old->pieces, 2 * 6 * sizeof(u64));
	b->color = !old->color;

	u64 p = pieceOn(old, m->src, old->color);

	if (p == PAWN) {
		if (old->color == UPPER && m->dst >= 7*8) p = QUEEN;
		if (old->color == LOWER && m->dst <  1*8) p = QUEEN;
	}

	clearPos(b, m->src);
	clearPos(b, m->dst);
	b->pieces[!b->color][p] |= ones(m->dst);

	refreshOcc(b);
}

