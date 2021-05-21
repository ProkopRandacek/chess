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
	// generate occ masks for both sides
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

int safePieceOn(Board* b, u8 pos) {
	for (u8 c = 0; c < 2; c++)
		for (u8 p = 0; p < 6; p++)
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
	memcpy(b, old, sizeof(Board));
	b->color = !old->color;

	u64 p = pieceOn(old, m->src, old->color);
	b->pieces[old->color][p] &= ~ones(m->src); // clear the pos that we are moving the piece from

	b->occ[old->color] &= ~ones(m->src); // clear the occ mask
	b->occ[b->color] |= ones(m->dst); // update occ mask

	if (p == PAWN) { // pawn promotion at the moment only to queen
		if (old->color == UPPER) {
			if (m->dst >= 7*8) p = QUEEN;
		}
		else if (m->dst < 1*8) p = QUEEN; // if color is not UPPER then it has to be LOWER and we dont need to check it
	}

	clearPos(b, m->dst); // remove the piece were capturing
	b->pieces[!b->color][p] |= ones(m->dst); // paste new piece on that position
}

