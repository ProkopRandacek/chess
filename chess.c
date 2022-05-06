#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <ucw/lib.h>
#include <ucw/gary.h>

#define one(x)       (((uint64_t)1) << (x))
#define one_sq(x, y) one((x) + (y)*8)
#define clz(x)       __builtin_clzl(x)
#define ctz(x)       __builtin_ctzl(x)
#define popcount(x)  __builtin_popcountl(x)

#define IS_UPPER(x) (((x)>='A') && ((x)<='Z'))
#define IS_LOWER(x) (((x)>='a') && ((x)<='z'))
#define IS_DIGIT(x) (((x)>='0') && ((x)<='9'))

//! Prints a formatted string.
#define log(fmt, ...) printf("%s:%d (%s) - " fmt "\n", __FILE__, __LINE__, __func__, ## __VA_ARGS__)

//! Prints a formatted string and dies.
#define err(fmt, ...) log(fmt, ## __VA_ARGS__),exit(1)

//! Die if false
#define assert(EXR) do { if (unlikely(!(EXR))) { log("assertion failed (" #EXR ")"); exit(1); } } while(0)

typedef uint64_t bb;

enum side {
	sWHITE = 1,
	sBLACK = 0
};

enum board_castle {
	// castling rights
	bcWL = 0b0001,
	bcWR = 0b0010,
	bcBL = 0b0100,
	bcBR = 0b1000
};

enum move_flags {
	mfNONE = 0,
	mfCASTLE,
	mfENPASS,
	mfPROMOTE,
};

enum piece {
	pPAWN   = 0,
	pKNIGHT = 1,
	pBISHOP = 2,
	pROOK   = 3,
	pQUEEN  = 4,
	pKING   = 5,
	pNONE   = 6,
};

struct move {
	u8 src;
	u8 dst;
	enum piece moving_piece;
	enum piece capturing_piece; // can be none
	enum move_flags flags:2;
	enum board_castle f:4;
};

struct board {
	bb pieces[2][6];
	bb occ[2];
	enum board_castle csl:4;
	bool clr;
};

const int piece_score[] = {
	[pBISHOP] = 300,
	[pKING]   = 0,
	[pKNIGHT] = 300,
	[pPAWN]   = 100,
	[pQUEEN]  = 900,
	[pROOK]   = 500,
};

char* piece_spelling[] = {
	[pBISHOP] = "bishop",
	[pKING]   = "king",
	[pKNIGHT] = "knight",
	[pNONE]   = "none",
	[pPAWN]   = "pawn",
	[pQUEEN]  = "queen",
	[pROOK]   = "rook",
};

const int checkmate_score = -10000;
const int center_score = 1;
const int move_score = 1;
const bb center = 0x00003C3C3C3C0000;
const bb pawn_start = 71776119061282560;

const bb promote_spots = 0b1111111100000000000000000000000000000000000000000000000011111111;

// bitboards
const bb ray_attacks[8][64] = {
	{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x101, 0x202, 0x404, 0x808, 0x1010, 0x2020, 0x4040, 0x8080, 0x10101, 0x20202, 0x40404, 0x80808, 0x101010, 0x202020, 0x404040, 0x808080, 0x1010101, 0x2020202, 0x4040404, 0x8080808, 0x10101010, 0x20202020, 0x40404040, 0x80808080, 0x101010101, 0x202020202, 0x404040404, 0x808080808, 0x1010101010, 0x2020202020, 0x4040404040, 0x8080808080, 0x10101010101, 0x20202020202, 0x40404040404, 0x80808080808, 0x101010101010, 0x202020202020, 0x404040404040, 0x808080808080, 0x1010101010101, 0x2020202020202, 0x4040404040404, 0x8080808080808, 0x10101010101010, 0x20202020202020, 0x40404040404040, 0x80808080808080 },
	{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x0, 0x204, 0x408, 0x810, 0x1020, 0x2040, 0x4080, 0x8000, 0x0, 0x20408, 0x40810, 0x81020, 0x102040, 0x204080, 0x408000, 0x800000, 0x0, 0x2040810, 0x4081020, 0x8102040, 0x10204080, 0x20408000, 0x40800000, 0x80000000, 0x0, 0x204081020, 0x408102040, 0x810204080, 0x1020408000, 0x2040800000, 0x4080000000, 0x8000000000, 0x0, 0x20408102040, 0x40810204080, 0x81020408000, 0x102040800000, 0x204080000000, 0x408000000000, 0x800000000000, 0x0, 0x2040810204080, 0x4081020408000, 0x8102040800000, 0x10204080000000, 0x20408000000000, 0x40800000000000, 0x80000000000000, 0x0 },
	{ 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x0, 0xfe00, 0xfc00, 0xf800, 0xf000, 0xe000, 0xc000, 0x8000, 0x0, 0xfe0000, 0xfc0000, 0xf80000, 0xf00000, 0xe00000, 0xc00000, 0x800000, 0x0, 0xfe000000, 0xfc000000, 0xf8000000, 0xf0000000, 0xe0000000, 0xc0000000, 0x80000000, 0x0, 0xfe00000000, 0xfc00000000, 0xf800000000, 0xf000000000, 0xe000000000, 0xc000000000, 0x8000000000, 0x0, 0xfe0000000000, 0xfc0000000000, 0xf80000000000, 0xf00000000000, 0xe00000000000, 0xc00000000000, 0x800000000000, 0x0, 0xfe000000000000, 0xfc000000000000, 0xf8000000000000, 0xf0000000000000, 0xe0000000000000, 0xc0000000000000, 0x80000000000000, 0x0, 0xfe00000000000000, 0xfc00000000000000, 0xf800000000000000, 0xf000000000000000, 0xe000000000000000, 0xc000000000000000, 0x8000000000000000, 0x0 },
	{ 0x8040201008040200, 0x80402010080400, 0x804020100800, 0x8040201000, 0x80402000, 0x804000, 0x8000, 0x0, 0x4020100804020000, 0x8040201008040000, 0x80402010080000, 0x804020100000, 0x8040200000, 0x80400000, 0x800000, 0x0, 0x2010080402000000, 0x4020100804000000, 0x8040201008000000, 0x80402010000000, 0x804020000000, 0x8040000000, 0x80000000, 0x0, 0x1008040200000000, 0x2010080400000000, 0x4020100800000000, 0x8040201000000000, 0x80402000000000, 0x804000000000, 0x8000000000, 0x0, 0x804020000000000, 0x1008040000000000, 0x2010080000000000, 0x4020100000000000, 0x8040200000000000, 0x80400000000000, 0x800000000000, 0x0, 0x402000000000000, 0x804000000000000, 0x1008000000000000, 0x2010000000000000, 0x4020000000000000, 0x8040000000000000, 0x80000000000000, 0x0, 0x200000000000000, 0x400000000000000, 0x800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
	{ 0x101010101010100, 0x202020202020200, 0x404040404040400, 0x808080808080800, 0x1010101010101000, 0x2020202020202000, 0x4040404040404000, 0x8080808080808000, 0x101010101010000, 0x202020202020000, 0x404040404040000, 0x808080808080000, 0x1010101010100000, 0x2020202020200000, 0x4040404040400000, 0x8080808080800000, 0x101010101000000, 0x202020202000000, 0x404040404000000, 0x808080808000000, 0x1010101010000000, 0x2020202020000000, 0x4040404040000000, 0x8080808080000000, 0x101010100000000, 0x202020200000000, 0x404040400000000, 0x808080800000000, 0x1010101000000000, 0x2020202000000000, 0x4040404000000000, 0x8080808000000000, 0x101010000000000, 0x202020000000000, 0x404040000000000, 0x808080000000000, 0x1010100000000000, 0x2020200000000000, 0x4040400000000000, 0x8080800000000000, 0x101000000000000, 0x202000000000000, 0x404000000000000, 0x808000000000000, 0x1010000000000000, 0x2020000000000000, 0x4040000000000000, 0x8080000000000000, 0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
	{ 0x0, 0x100, 0x10200, 0x1020400, 0x102040800, 0x10204081000, 0x1020408102000, 0x102040810204000, 0x0, 0x10000, 0x1020000, 0x102040000, 0x10204080000, 0x1020408100000, 0x102040810200000, 0x204081020400000, 0x0, 0x1000000, 0x102000000, 0x10204000000, 0x1020408000000, 0x102040810000000, 0x204081020000000, 0x408102040000000, 0x0, 0x100000000, 0x10200000000, 0x1020400000000, 0x102040800000000, 0x204081000000000, 0x408102000000000, 0x810204000000000, 0x0, 0x10000000000, 0x1020000000000, 0x102040000000000, 0x204080000000000, 0x408100000000000, 0x810200000000000, 0x1020400000000000, 0x0, 0x1000000000000, 0x102000000000000, 0x204000000000000, 0x408000000000000, 0x810000000000000, 0x1020000000000000, 0x2040000000000000, 0x0, 0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
	{ 0x0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0x0, 0x100, 0x300, 0x700, 0xf00, 0x1f00, 0x3f00, 0x7f00, 0x0, 0x10000, 0x30000, 0x70000, 0xf0000, 0x1f0000, 0x3f0000, 0x7f0000, 0x0, 0x1000000, 0x3000000, 0x7000000, 0xf000000, 0x1f000000, 0x3f000000, 0x7f000000, 0x0, 0x100000000, 0x300000000, 0x700000000, 0xf00000000, 0x1f00000000, 0x3f00000000, 0x7f00000000, 0x0, 0x10000000000, 0x30000000000, 0x70000000000, 0xf0000000000, 0x1f0000000000, 0x3f0000000000, 0x7f0000000000, 0x0, 0x1000000000000, 0x3000000000000, 0x7000000000000, 0xf000000000000, 0x1f000000000000, 0x3f000000000000, 0x7f000000000000, 0x0, 0x100000000000000, 0x300000000000000, 0x700000000000000, 0xf00000000000000, 0x1f00000000000000, 0x3f00000000000000, 0x7f00000000000000 },
	{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x0, 0x100, 0x201, 0x402, 0x804, 0x1008, 0x2010, 0x4020, 0x0, 0x10000, 0x20100, 0x40201, 0x80402, 0x100804, 0x201008, 0x402010, 0x0, 0x1000000, 0x2010000, 0x4020100, 0x8040201, 0x10080402, 0x20100804, 0x40201008, 0x0, 0x100000000, 0x201000000, 0x402010000, 0x804020100, 0x1008040201, 0x2010080402, 0x4020100804, 0x0, 0x10000000000, 0x20100000000, 0x40201000000, 0x80402010000, 0x100804020100, 0x201008040201, 0x402010080402, 0x0, 0x1000000000000, 0x2010000000000, 0x4020100000000, 0x8040201000000, 0x10080402010000, 0x20100804020100, 0x40201008040201 }
};
const bb king_attacks[64] = { 0x302, 0x705, 0xe0a, 0x1c14, 0x3828, 0x7050, 0xe0a0, 0xc040, 0x30203, 0x70507, 0xe0a0e, 0x1c141c, 0x382838, 0x705070, 0xe0a0e0, 0xc040c0, 0x3020300, 0x7050700, 0xe0a0e00, 0x1c141c00, 0x38283800, 0x70507000, 0xe0a0e000, 0xc040c000, 0x302030000, 0x705070000, 0xe0a0e0000, 0x1c141c0000, 0x3828380000, 0x7050700000, 0xe0a0e00000, 0xc040c00000, 0x30203000000, 0x70507000000, 0xe0a0e000000, 0x1c141c000000, 0x382838000000, 0x705070000000, 0xe0a0e0000000, 0xc040c0000000, 0x3020300000000, 0x7050700000000, 0xe0a0e00000000, 0x1c141c00000000, 0x38283800000000, 0x70507000000000, 0xe0a0e000000000, 0xc040c000000000, 0x302030000000000, 0x705070000000000, 0xe0a0e0000000000, 0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000, 0x203000000000000, 0x507000000000000, 0xa0e000000000000, 0x141c000000000000, 0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000 };
const bb knight_attacks[64] = { 0x20400, 0x50800, 0xa1100, 0x142200, 0x284400, 0x508800, 0xa01000, 0x402000, 0x2040004, 0x5080008, 0xa110011, 0x14220022, 0x28440044, 0x50880088, 0xa0100010, 0x40200020, 0x204000402, 0x508000805, 0xa1100110a, 0x1422002214, 0x2844004428, 0x5088008850, 0xa0100010a0, 0x4020002040, 0x20400040200, 0x50800080500, 0xa1100110a00, 0x142200221400, 0x284400442800, 0x508800885000, 0xa0100010a000, 0x402000204000, 0x2040004020000, 0x5080008050000, 0xa1100110a0000, 0x14220022140000, 0x28440044280000, 0x50880088500000, 0xa0100010a00000, 0x40200020400000, 0x204000402000000, 0x508000805000000, 0xa1100110a000000, 0x1422002214000000, 0x2844004428000000, 0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000, 0x400040200000000, 0x800080500000000, 0x1100110a00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000, 0x100010a000000000, 0x2000204000000000, 0x4020000000000, 0x8050000000000, 0x110a0000000000, 0x22140000000000, 0x44280000000000, 0x88500000000000, 0x10a00000000000, 0x20400000000000 };
const bb pawn_attacks[2][64] = {
	{ 0x200, 0x500, 0xa00, 0x1400, 0x2800, 0x5000, 0xa000, 0x4000, 0x20000, 0x50000, 0xa0000, 0x140000, 0x280000, 0x500000, 0xa00000, 0x400000, 0x2000000, 0x5000000, 0xa000000, 0x14000000, 0x28000000, 0x50000000, 0xa0000000, 0x40000000, 0x200000000, 0x500000000, 0xa00000000, 0x1400000000, 0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000, 0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000, 0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000, 0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000, 0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000, 0x200000000000000, 0x500000000000000, 0xa00000000000000, 0x1400000000000000, 0x2800000000000000, 0x5000000000000000, 0xa000000000000000, 0x4000000000000000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
	{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x5, 0xa, 0x14, 0x28, 0x50, 0xa0, 0x40, 0x200, 0x500, 0xa00, 0x1400, 0x2800, 0x5000, 0xa000, 0x4000, 0x20000, 0x50000, 0xa0000, 0x140000, 0x280000, 0x500000, 0xa00000, 0x400000, 0x2000000, 0x5000000, 0xa000000, 0x14000000, 0x28000000, 0x50000000, 0xa0000000, 0x40000000, 0x200000000, 0x500000000, 0xa00000000, 0x1400000000, 0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000, 0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000, 0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000, 0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000, 0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000 }
};

int bit_scan(bb b, bool reverse) {
	bb r_mask = -(bb)reverse;
	b &= -b | r_mask;
	return (63 - clz(b));
}

enum piece char2piece(char c) {
	if (c == 'p' || c == 'P') return pPAWN;
	if (c == 'r' || c == 'R') return pROOK;
	if (c == 'n' || c == 'N') return pKNIGHT;
	if (c == 'b' || c == 'B') return pBISHOP;
	if (c == 'k' || c == 'K') return pKING;
	if (c == 'q' || c == 'Q') return pQUEEN;
	assert(false);
}

///////////
// BOARD //
///////////

void board_load_fen(struct board* b, const char* fen) {
	memset(b, 0, sizeof(struct board));

	b->csl |= (bcWL | bcWR | bcBL | bcBR); // toggle castle rights on

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
	b->clr = fen[head] == 'w' ? sWHITE : sBLACK;

	// generate occ masks for both sides
	b->occ[0] = b->pieces[0][0] | b->pieces[0][1] | b->pieces[0][2] | b->pieces[0][3] | b->pieces[0][4] | b->pieces[0][5];
	b->occ[1] = b->pieces[1][0] | b->pieces[1][1] | b->pieces[1][2] | b->pieces[1][3] | b->pieces[1][4] | b->pieces[1][5];
}

//! lr -> 0=left side, 1=right side
inline bool can_csl(struct board* b, enum side s, bool lr) {
	return b->csl & (1 << (2*s+lr));
}

enum piece piece_on(struct board* b, int pos, int color) {
	for (int i = 0; i < 6; i++)
		if (b->pieces[color][i] & one(pos))
			return (enum piece)i;
	assert(false);
}

void make_move(struct board* b, struct move* m) {
	enum piece p = m->moving_piece; // the piece that we should place

	b->pieces[b->clr][p] &= ~one(m->src); // clear the pos that we are moving the piece from

	b->occ[b->clr] &= ~one(m->src); // remove the source from occ
	b->occ[b->clr] |= one(m->dst); // add the destination to occ

	if (m->flags == mfPROMOTE)
		p = pQUEEN;

	if (m->flags == mfCASTLE) {
		bb add, remove;
		switch (m->dst) {
			case 62: add = 61; remove = 63; break;
			case 58: add = 59; remove = 56; break;
			case 6:  add = 5;  remove = 7;  break;
			case 2:  add = 3;  remove = 0;  break;
			default: assert(false);
		}
		b->pieces[b->clr][pROOK] |= one(add);
		b->pieces[b->clr][pROOK] &= ~one(remove);
		b->occ[b->clr] |= one(add);
		b->occ[b->clr] &= ~one(remove);
	}

	b->pieces[b->clr][p] |= one(m->dst); // paste new piece on that position

	if (m->capturing_piece != pNONE) { // if we are capturing, remove that piece
		b->pieces[!b->clr][m->capturing_piece] &= ~one(m->dst);
		b->occ[!b->clr] &= ~one(m->dst); // clear enemy the occ mask
	}

	// taking away castling rights
	if (!(b->occ[sWHITE] & one(60))) b->csl &= ~(bcWL|bcWR);
	if (!(b->occ[sWHITE] & one(56))) b->csl &= ~bcWL;
	if (!(b->occ[sWHITE] & one(63))) b->csl &= ~bcWR;

	if (!(b->occ[sBLACK] & one(4))) b->csl &= ~(bcBL|bcBR);
	if (!(b->occ[sBLACK] & one(0))) b->csl &= ~bcBL;
	if (!(b->occ[sBLACK] & one(7))) b->csl &= ~bcBR;

	b->clr = !b->clr;
}

void unmake_move(struct board* b, struct move* m) {
	b->clr = !b->clr;

	enum piece p = m->moving_piece;

	b->occ[b->clr] &= ~one(m->dst); // remove the dst from occ
	b->occ[b->clr] |= one(m->src); // add the src to occ

	b->pieces[b->clr][p] |= one(m->src); // add the place that we are moving from

	if (m->flags == mfPROMOTE) {
		p = pQUEEN;
	}
	if (m->flags == mfCASTLE) {
		bb add, remove;
		switch (m->dst) {
			case 62: remove = 61; add = 63; break;
			case 58: remove = 59; add = 56; break;
			case 6:  remove = 5;  add = 7;  break;
			case 2:  remove = 3;  add = 0;  break;
			default: assert(false);
		}
		b->pieces[b->clr][pROOK] |= one(add);
		b->pieces[b->clr][pROOK] &= ~one(remove);
		b->occ[b->clr] |= one(add);
		b->occ[b->clr] &= ~one(remove);
	}

	b->pieces[b->clr][p] &= ~one(m->dst); // clear the pos that we moved to

	if (m->capturing_piece != pNONE) {
		b->pieces[!b->clr][m->capturing_piece] |= one(m->dst);
		b->occ[!b->clr]|= one(m->dst);
	}
}

//////////////
// MOVE GEN //
//////////////

bb get_ray_bb(bb occ, int dir, int pos) {
	bb att = ray_attacks[dir][pos];
	bb blockers = att & occ;
	if (blockers) {
		int c = bit_scan(blockers, dir >= 6 || dir <= 1);
		att = att ^ ray_attacks[dir][c];
	}
	return att;
}

bb gen_b_moves_bb(int sq, struct board* b) {
	bb occ = b->occ[0] | b->occ[1];
	return (get_ray_bb(occ, 1, sq) | get_ray_bb(occ, 3, sq) | get_ray_bb(occ, 5, sq) | get_ray_bb(occ, 7, sq)) & ~b->occ[b->clr];
}

bb gen_r_moves_bb(int sq, struct board* b) {
	bb occ = b->occ[0] | b->occ[1];
	return (get_ray_bb(occ, 0, sq) | get_ray_bb(occ, 2, sq) | get_ray_bb(occ, 4, sq) | get_ray_bb(occ, 6, sq)) & ~b->occ[b->clr];
}

bb gen_q_moves_bb(int sq, struct board* b) {
	bb occ = b->occ[0] | b->occ[1];
	return (get_ray_bb(occ, 0, sq) | get_ray_bb(occ, 1, sq) | get_ray_bb(occ, 2, sq) | get_ray_bb(occ, 3, sq) | get_ray_bb(occ, 4, sq) | get_ray_bb(occ, 5, sq) | get_ray_bb(occ, 6, sq) | get_ray_bb(occ, 7, sq)) & ~b->occ[b->clr];
}

bb gen_k_moves_bb(int sq, struct board* b) {
	return king_attacks[sq] & ~b->occ[b->clr];
}

bb gen_n_moves_bb(int sq, struct board* b) {
	return knight_attacks[sq] & ~b->occ[b->clr];
}

bb gen_p_moves_bb(int sq, struct board* b) {
	bb occ = b->occ[0] | b->occ[1];
	bb att = pawn_attacks[b->clr][sq] & b->occ[!b->clr];

	if (b->clr) {
		att |= (one(sq) >> 8) & ~occ;
		if ((one(sq) & pawn_start) && !((one(sq) >> 8 | one(sq) >> 16) & occ))
			att |= one(sq) >> 16;
	} else {
		att |= (one(sq) << 8) & ~occ;
		if ((one(sq) & pawn_start) && !((one(sq) << 8 | one(sq) << 16) & occ))
			att |= one(sq) << 16;
	}
	return att;
}

bb only_capture_bb(bb b, struct board* brd) {
	return b & brd->occ[!brd->clr];
}

bb only_quiet_bb(bb b, struct board* brd) {
	return b & ~brd->occ[!brd->clr];
}

bb (*gen_moves_bb[6]) (int sq, struct board* b) = { gen_p_moves_bb, gen_n_moves_bb, gen_b_moves_bb, gen_r_moves_bb, gen_q_moves_bb, gen_k_moves_bb };

bool is_square_attacked(struct board* b, int pos) {
	for (int i = 0; i < 6; i++)
		if (gen_moves_bb[i](pos, b) & b->pieces[!b->clr][i])
			return true;
	return false;
}

bool leaves_in_check(struct board* b, struct move* m) {
	make_move(b, m);

	int my_king = ctz(b->pieces[!b->clr][pKING]); // ctz converts bb with popcount of 1 to square index
	b->clr = !b->clr;

	bool a = is_square_attacked(b, my_king);

	b->clr = !b->clr;
	unmake_move(b, m);
	return a;
}

void bb2moves(int src, bb b, struct board* board, enum piece mp, enum piece cap, enum move_flags f, struct move** m) {
	if (!b)
		return;

	for (int i = ctz(b); i < (64 - clz(b)); i++) {
		if (!(one(i) & b))
			continue;

		struct move mov = (struct move){src, i, mp, cap, f, board->csl};

		if ((mov.moving_piece == pPAWN) && (one(mov.dst) & promote_spots)) {
			mov.flags = mfPROMOTE;
		}

		if (!leaves_in_check(board, &mov)) {
			*GARY_PUSH(*m) = mov;
		}
	}
}

void gen_legal_moves(struct board* b, struct move** m) {
	for (int p = 0; p < 6; p++) { // for all piece types
		bb pbb = b->pieces[b->clr][p];

		int step = 1;
		if (popcount(pbb) == 2) // skip empty squares between 2 pieces
			step = (64 - clz(pbb)) - ctz(pbb) - 1;

		if (pbb) {
			for (int i = ctz(pbb); i < (64 - clz(pbb)); i+=step) {
				if (one(i) & pbb) {
					bb movebb = gen_moves_bb[p](i, b);

					bb2moves(i, movebb & b->pieces[!b->clr][pBISHOP], b, p, pBISHOP, 0, m); // bishop captures
					bb2moves(i, movebb & b->pieces[!b->clr][pROOK  ], b, p, pROOK  , 0, m); // rook captures
					bb2moves(i, movebb & b->pieces[!b->clr][pQUEEN ], b, p, pQUEEN , 0, m); // ...
					bb2moves(i, movebb & b->pieces[!b->clr][pKING  ], b, p, pKING  , 0, m);
					bb2moves(i, movebb & b->pieces[!b->clr][pKNIGHT], b, p, pKNIGHT, 0, m);
					bb2moves(i, movebb & b->pieces[!b->clr][pPAWN  ], b, p, pPAWN  , 0, m);
					bb2moves(i, only_quiet_bb(movebb, b)                      , b, p, pNONE  , 0, m); // not capturing moves
				}
			}
		}
	}

	// castling
	if (b->clr == sWHITE) {
		if (b->csl & bcWL) { // king and rook haven't moved
			if (!((b->occ[!b->clr]|b->occ[b->clr]) & one(59))) { // the between squares are empty
				if (
						!is_square_attacked(b, 60) &&
						!is_square_attacked(b, 59) &&
						!is_square_attacked(b, 58)
				   ) { // the squares are not attacked
					*GARY_PUSH(*m) = (struct move) { 60, 58, pKING, pNONE, mfCASTLE, b->csl };
				}
			}
		}

		if (b->csl & bcWR) {
			if (!((b->occ[!b->clr]|b->occ[b->clr]) & one(61))) {
				if (
						!is_square_attacked(b, 60) &&
						!is_square_attacked(b, 61) &&
						!is_square_attacked(b, 62)
				   ) { // the squares are not attacked
					*GARY_PUSH(*m) = (struct move) { 60, 62, pKING, pNONE, mfCASTLE, b->csl };
				}
			}
		}
	} else {
		if (b->csl & bcBL) {
			if (!((b->occ[!b->clr]|b->occ[b->clr]) & one(3))) {
				if (
						!is_square_attacked(b, 2) &&
						!is_square_attacked(b, 3) &&
						!is_square_attacked(b, 4)
				   ) { // the squares are not attacked
					*GARY_PUSH(*m) = (struct move) { 4, 2, pKING, pNONE, mfCASTLE, b->csl };
				}
			}
		}

		if (b->csl & bcBR) {
			if (!((b->occ[!b->clr]|b->occ[b->clr]) & one(5))) {
				if (
						!is_square_attacked(b, 4) &&
						!is_square_attacked(b, 5) &&
						!is_square_attacked(b, 6)
				   ) { // the squares are not attacked
					*GARY_PUSH(*m) = (struct move) { 4, 6, pKING, pNONE, mfCASTLE, b->csl };
				}
			}
		}
	}
}

//////////
// EVAL //
//////////

int eval_center(struct board* b) {
	return center_score * popcount(b->occ[b->clr] & center);
}

int piece_diff(struct board* b, int piece) {
	return popcount(b->pieces[b->clr][piece]) - popcount(b->pieces[!b->clr][piece]);
}

int eval_pieces(struct board* b) {
	int score = 0;
	for (int p = 0; p < 5; p++)
		score += piece_score[p] * piece_diff(b, p);
	return score;
}

int eval_moves(int moveCount) {
	if (moveCount == 0) return checkmate_score;
	return (int)moveCount * move_score;
}

int eval(struct board* b, int moveCount) {
	return eval_pieces(b) + eval_center(b) + eval_moves(moveCount);
}

//////////////////
// TREE WALKING //
//////////////////

int _perft(struct board* b, int d) {
	struct move* moves;
	int* frames; // indexes where frames start
	GARY_INIT(moves, 0);
	GARY_INIT(frames, 1);
	frames[0] = 0;
	gen_legal_moves(b, &moves);

	int sum = 1;

	while (1) {
		int mlast = GARY_SIZE(moves)-1;
		int flast = GARY_SIZE(frames)-1;

		if (mlast == frames[flast]) { // if we entered previous frame
			if (flast == 0) {
				break;
			}

			GARY_POP(frames);
			struct move* m = &moves[mlast];
			unmake_move(b, m);
			GARY_POP(moves);
		} else { // we are inside a frame
			if (flast+1 == d) { // if we are too deep
				sum++;
				GARY_POP(moves);
			} else { // expand into lower frame
				struct move* m = &moves[mlast]; // take the move

				make_move(b, m); // expand the moves from this move
				gen_legal_moves(b, &moves);

				*GARY_PUSH(frames) = mlast; // points to the first move that expanded into this frame
			}
		}
	}

	GARY_FREE(moves);
	return sum;
}

void perft(struct board* b, int d) {
	if (d == 0)
		return;

	struct move* moves;
	GARY_INIT(moves, 0);
	gen_legal_moves(b, &moves);
	size_t moves_count = GARY_SIZE(moves);

	for (size_t i = 0; i < moves_count; i++) {
		struct move* m = &moves[i];
		make_move(b, m);
		int sum = _perft(b, d-1);
		unmake_move(b, m);
		printf("%c%d%c%d - %d\n", (m->src%8)+'a', (7-m->src/8)+1, (m->dst%8)+'a', (7-m->dst/8)+1, sum);
	}
}

int search(struct board* s, int a, int b, int d) {
	struct move* moves;
	GARY_INIT_SPACE(moves, 20);
	gen_legal_moves(s, &moves);

	size_t moves_count = GARY_SIZE(moves);
	if (d == 0 || moves_count == 0) {
		GARY_FREE(moves);
		return eval(s, moves_count);
	}

	int score = checkmate_score;

	for (size_t pos = 0; pos <= moves_count; pos++) {
		make_move(s, &moves[pos]);
		score = -search(s, a, b, d-1);
		unmake_move(s, &moves[pos]);

		if (score >= b) {
			GARY_FREE(moves);
			return b;
		}

		a = MAX(a, score);
	}
	GARY_FREE(moves);
	return a;
}

//////////////
// PRINTING //
//////////////

void print_bb_line(bb board, int line) {
	for (int i = 8*line; i < 8*line+8; i++) {
		bool b = board & one(i) ? true : false;
		if (b) printf("\e[0;32m");
		printf(" %d", b);
		if (b) printf("\e[0m");
	}
}

void print_board(struct board* b) {
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
	printf("%s to move\n", b->clr == sBLACK ? "black" : "white");
	printf("csl: %d %d %d %d\n", (b->csl&1)!=0, (b->csl&2)!=0, (b->csl&4)!=0, (b->csl&8)!=0);
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

/////////
// GUI //
/////////

#include <raylib.h>
#include "assets/bb.png.h"
#include "assets/bk.png.h"
#include "assets/bn.png.h"
#include "assets/bp.png.h"
#include "assets/bq.png.h"
#include "assets/br.png.h"
#include "assets/wb.png.h"
#include "assets/wk.png.h"
#include "assets/wn.png.h"
#include "assets/wp.png.h"
#include "assets/wq.png.h"
#include "assets/wr.png.h"

#define TILE_SIZE 100
#define BOARD_SIZE (TILE_SIZE*8)

const unsigned char* tex_data[12] = { bp_png, bn_png, bb_png, br_png, bq_png, bk_png, wp_png, wn_png, wb_png, wr_png, wq_png, wk_png };
const int tex_size[12] = { bp_png_size, bn_png_size, bb_png_size, br_png_size, bq_png_size, bk_png_size, wp_png_size, wn_png_size, wb_png_size, wr_png_size, wq_png_size, wk_png_size };

Texture2D pieces[12];

void render_loop(struct board* b) {
	int hovering = -1; // the square under cursor

	bb hl_changed  = 0; // what piece moved since last time?
	bb hl_possible = 0; // what are the possible moves with this piece that he's holding?

	bool moving = false; // is user moving with a piece?
	int move_start = 0;
	int move_end = 0;
	enum piece move_piece = 0;

	Vector2 board_cursor = {0, 0}; // cursor position within the board. can be negative if the cursor is top/left of the board
	Vector2 cursor = {0, 0}; // cursor position within the window.

	while (!WindowShouldClose()) {
		cursor = GetMousePosition();
		board_cursor = (Vector2){
			cursor.x/BOARD_SIZE*8,
			cursor.y/BOARD_SIZE*8
		};

		if (board_cursor.x >= 0.0 && board_cursor.x <= 8.0 && board_cursor.y >= 0.0 && board_cursor.y <= 8.0) {
			hovering = ((int)board_cursor.x) + ((int)board_cursor.y * 8);
		} else {
			hovering = -1;
		}

		if (!moving && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && hovering != -1) {
			if (hovering >= 0 && b->occ[b->clr] & one(hovering)) {
				// Started moving a piece
				moving = true;
				move_start = hovering;
				move_piece = piece_on(b, hovering, b->clr);
				struct move* legal;
				GARY_INIT(legal, 0);
				gen_legal_moves(b, &legal);

				size_t l = GARY_SIZE(legal);

				hl_possible = 0;
				for (size_t i = 0; i < l; i++) {
					struct move* m = &legal[i];
					if (m->src == move_start)
						hl_possible |= one(m->dst);
				}
			}
		}
		if (moving && !IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			// stopped moving
			moving = false;
			hl_possible = 0;

			move_end = hovering;

			struct move* legal;
			GARY_INIT(legal, 0);
			gen_legal_moves(b, &legal);

			size_t l = GARY_SIZE(legal);

			bool ok = false;
			struct move mov;
			for (size_t i = 0; i < l; i++) {
				struct move* m = &legal[i];
				if (m->src == move_start && m->dst == move_end) {
					ok = true;
					mov = *m;
				}
			}

			if (ok) {
				print_move(&mov);
				hl_changed = one(mov.src)|one(mov.dst);
				make_move(b, &mov);
				print_board(b);
			}

			GARY_FREE(legal);

			// execute AI move
		}

		BeginDrawing(); {
			ClearBackground(GRAY);

			// Draw grid
			for (int y = 0; y < 8; y++)
				for (int x = 0; x < 8; x++) {
					Rectangle rec = (Rectangle){x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE};
					DrawRectangleRec(rec, (x+y)&1 ? (Color){238,238,210,255} : (Color){118,150,86,255});

					if (hl_changed & one_sq(x, y)) DrawRectangleLinesEx(rec, TILE_SIZE/20, (Color){186,202,68,100});

					if (((b->occ[0]|b->occ[1])&one_sq(x, y)) && !(moving && (move_start == y*8+x))) {
						enum side s = b->occ[sWHITE] & one_sq(x, y) ? sWHITE : sBLACK;
						DrawTexture(pieces[piece_on(b, y*8+x, s) + 6*s], x*TILE_SIZE, y*TILE_SIZE, WHITE);
					}

					if (hl_possible & one_sq(x, y)) DrawCircle(x*TILE_SIZE + TILE_SIZE/2, y*TILE_SIZE + TILE_SIZE/2, TILE_SIZE/7, (Color){186,202,68,100});
				}
			// moving piece
			if (moving) DrawTexture(pieces[move_piece + 6*(b->clr)], (int)cursor.x - TILE_SIZE/2, (int)cursor.y - TILE_SIZE / 2, WHITE);

		} EndDrawing();
	}
}

int gui(struct board* b) {
	SetTraceLogLevel(-10);
	InitWindow(BOARD_SIZE, BOARD_SIZE, "chess");
	SetTargetFPS(60);

	for (int i = 0; i < 12; i++) {
		Image img = LoadImageFromMemory(".png", tex_data[i], tex_size[i]);
		ImageResize(&img, TILE_SIZE, TILE_SIZE);
		pieces[i] = LoadTextureFromImage(img);
	}

	render_loop(b);

	for (int i = 0; i < 12; i++)
		UnloadTexture(pieces[i]);

	CloseWindow();

	return 0;
}

//////////
// MAIN //
//////////

int main() {
	struct board b;
	board_load_fen(&b, "rnbqkbnr/pppppppp/////PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	print_board(&b);

#if 1
	gui(&b);
#else
	int ply = 4;
	perft(&b, ply);
#endif
	print_board(&b);
}

