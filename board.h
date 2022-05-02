#pragma once

#include "common.h"

#include "move.h"

enum side {
	sWHITE = 1,
	sBLACK = 0
};

struct board {
	bb pieces[2][6];
	bb occ[2];
	enum side color_to_move;
	bool caste_white_left;
	bool caste_white_right;
	bool caste_black_left;
	bool caste_black_right;
};

void   make_move(struct board* b, struct move*);
void unmake_move(struct board* b, struct move*);

enum piece piece_on(struct board* b, int pos, int color);

