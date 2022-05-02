#include "strmanip.h"

#include <string.h>

enum piece char2piece(char c) {
	if (c == 'p' || c == 'P') return pPAWN;
	if (c == 'r' || c == 'R') return pROOK;
	if (c == 'n' || c == 'N') return pKNIGHT;
	if (c == 'b' || c == 'B') return pBISHOP;
	if (c == 'k' || c == 'K') return pKING;
	if (c == 'q' || c == 'Q') return pQUEEN;
	assert(false);
}

void bb2char(bb bb, char board[8][8], char c) {
	if (bb) {
		for (int i = ctz(bb); i < (64 - clz(bb)); i++)
			if (bb & one(i))
				board[i / 8][i % 8] = c;
	}
}

char* piece_spelling[] = {
	[pPAWN]   = "pawn",
	[pKNIGHT] = "knight",
	[pBISHOP] = "bishop",
	[pROOK]   = "rook",
	[pQUEEN]  = "queen",
	[pKING]   = "king",
	[pNONE]   = "none"
};


void board_load_fen(struct board* b, const char* fen) {
	memset(b, 0, sizeof(struct board));

	b->caste_white_left = true;
	b->caste_white_right = true;
	b->caste_black_left = true;
	b->caste_black_right = true;

	int x = 0, y = 0;
	int head = 0;

	while (fen[head] != ' ') {
		if (fen[head] == '/') {
			y++;
			x = 0;
		} else {
			if (fen[head] >= '0' && fen[head] <= '9') {
				x += (char)(fen[head] - 48);
			} else {
				int c = (fen[head] >= 'A' && fen[head] <= 'Z') ? sWHITE : sBLACK;
				b->pieces[c][char2piece(fen[head])] |= one_sq(x, y);
				x++;
			}
		}
		head++;
	}
	head++; // skip space
	b->color_to_move = fen[head] == 'w' ? sWHITE : sBLACK;

	// generate occ masks for both sides
	b->occ[0] = b->pieces[0][0] | b->pieces[0][1] | b->pieces[0][2] | b->pieces[0][3] | b->pieces[0][4] | b->pieces[0][5];
	b->occ[1] = b->pieces[1][0] | b->pieces[1][1] | b->pieces[1][2] | b->pieces[1][3] | b->pieces[1][4] | b->pieces[1][5];
}

void print_bb_line(bb board, int line) {
	for (int i = 8*line; i < 8*line+8; i++) {
		bool b = board & one(i) ? true : false;
		if (b) printf("\e[0;32m");
		printf(" %d", b);
		if (b) printf("\e[0m");
	}
}

void board_print(struct board* b) {
	for (int s = 0; s <= 1; s++) {
		printf("%s\n   OCC               PAWN              KNIGHT            BISHOP            ROOK              QUEEN             KING\n", s == sBLACK ? "BLACK" : "WHITE");
		for (int line = 0; line < 8; line++) {
			printf("  "),print_bb_line(b->occ[s],             line);
			printf("  "),print_bb_line(b->pieces[s][pPAWN],   line);
			printf("  "),print_bb_line(b->pieces[s][pKNIGHT], line);
			printf("  "),print_bb_line(b->pieces[s][pBISHOP], line);
			printf("  "),print_bb_line(b->pieces[s][pROOK],   line);
			printf("  "),print_bb_line(b->pieces[s][pQUEEN],  line);
			printf("  "),print_bb_line(b->pieces[s][pKING],   line);
			printf("\n");
		}
	}
	printf("%s to move\n", b->color_to_move == sBLACK ? "black" : "white");
	printf("%d %d %d %d\n", b->caste_white_left, b->caste_white_right, b->caste_black_left, b->caste_black_right);
}

void print_bb(bb board) {
	for (int i = 0; i < 64; i++) {
		bool b = board & one(i) ? true : false;
		if (b) printf("\e[0;32m");
		printf(" %d", b);
		if (b) printf("\e[0m");
		if (i%8 == 7) printf("\n");
	}
	printf("\n");
}

void print_move(struct move* m) {
	printf("%c%d%c%d - ", (m->src%8)+'a', (7-m->src/8)+1, (m->dst%8)+'a', (7-m->dst/8)+1 );
	printf("%s\t%s\t%d\n", piece_spelling[m->moving_piece], piece_spelling[m->capturing_piece], m->flags);
}

