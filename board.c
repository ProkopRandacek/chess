#include "board.h"

#include <string.h>
#include <stdlib.h>

enum piece piece_on(struct board* b, int pos, int color) {
	for (int i = 0; i < 6; i++)
		if (b->pieces[color][i] & one(pos))
			return (enum piece)i;
	assert(false);
}

void make_move(struct board* b, struct move* m) {
	enum piece p = m->moving_piece; // the piece that we should place

	b->pieces[b->color_to_move][p] &= ~one(m->src); // clear the pos that we are moving the piece from

	b->occ[b->color_to_move] &= ~one(m->src); // remove the source from occ
	b->occ[b->color_to_move] |= one(m->dst); // add the destination to occ

	if (m->flags & mfPROMOTE) {
		p = pQUEEN;
	}
	if (m->flags & mfCASTLE) {
		bb add, remove;
		switch (m->dst) {
			case 62: add = 61; remove = 63; break;
			case 58: add = 59; remove = 56; break;
			case 6:  add = 5;  remove = 7;  break;
			case 2:  add = 3;  remove = 0;  break;
			default: assert(false);
		}
		b->pieces[b->color_to_move][pROOK] |= one(add);
		b->pieces[b->color_to_move][pROOK] &= ~one(remove);
		b->occ[b->color_to_move] |= one(add);
		b->occ[b->color_to_move] &= ~one(remove);
		if (b->color_to_move != sBLACK) {
			b->caste_black_left = false;
			b->caste_black_right = false;
		} else {
			b->caste_white_left = false;
			b->caste_white_right = false;
		}
	}

	b->pieces[b->color_to_move][p] |= one(m->dst); // paste new piece on that position

	if (m->capturing_piece != pNONE) { // if we are capturing, remove that piece
		b->pieces[!b->color_to_move][m->capturing_piece] &= ~one(m->dst);
		b->occ[!b->color_to_move] &= ~one(m->dst); // clear enemy the occ mask
	}


	b->color_to_move = !b->color_to_move;
}

void unmake_move(struct board* b, struct move* m) {
	b->color_to_move = !b->color_to_move;

	enum piece p = m->moving_piece;

	b->occ[b->color_to_move] &= ~one(m->dst); // remove the dst from occ
	b->occ[b->color_to_move] |= one(m->src); // add the src to occ

	b->pieces[b->color_to_move][p] |= one(m->src); // add the place that we are moving from

	if (m->flags & mfPROMOTE) {
		p = pQUEEN;
	}
	if (m->flags & mfCASTLE) {
		bb add, remove;
		switch (m->dst) {
			case 62: remove = 61; add = 63; break;
			case 58: remove = 59; add = 56; break;
			case 6:  remove = 5;  add = 7;  break;
			case 2:  remove = 3;  add = 0;  break;
			default: assert(false);
		}
		b->pieces[b->color_to_move][pROOK] |= one(add);
		b->pieces[b->color_to_move][pROOK] &= ~one(remove);
		b->occ[b->color_to_move] |= one(add);
		b->occ[b->color_to_move] &= ~one(remove);
	}

	b->pieces[b->color_to_move][p] &= ~one(m->dst); // clear the pos that we moved to

	if (m->capturing_piece != pNONE) {
		b->pieces[!b->color_to_move][m->capturing_piece] |= one(m->dst);
		b->occ[!b->color_to_move]|= one(m->dst);
	}
}

