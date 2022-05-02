#include "movegen.h"
#include "board.h"
#include "table.h"

#include <ucw/lib.h>
#include <ucw/gary.h>
#include <string.h>

int bit_scan(bb b, bool reverse) {
	bb r_mask = -(bb)reverse;
	b &= -b | r_mask;
	return (63 - clz(b));
}

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
	return (get_ray_bb(occ, 1, sq) | get_ray_bb(occ, 3, sq) | get_ray_bb(occ, 5, sq) | get_ray_bb(occ, 7, sq)) & ~b->occ[b->color_to_move];
}

bb gen_r_moves_bb(int sq, struct board* b) {
	bb occ = b->occ[0] | b->occ[1];
	return (get_ray_bb(occ, 0, sq) | get_ray_bb(occ, 2, sq) | get_ray_bb(occ, 4, sq) | get_ray_bb(occ, 6, sq)) & ~b->occ[b->color_to_move];
}

bb gen_q_moves_bb(int sq, struct board* b) {
	bb occ = b->occ[0] | b->occ[1];
	return (get_ray_bb(occ, 0, sq) | get_ray_bb(occ, 1, sq) | get_ray_bb(occ, 2, sq) | get_ray_bb(occ, 3, sq) | get_ray_bb(occ, 4, sq) | get_ray_bb(occ, 5, sq) | get_ray_bb(occ, 6, sq) | get_ray_bb(occ, 7, sq)) & ~b->occ[b->color_to_move];
}

bb gen_k_moves_bb(int sq, struct board* b) {
	return king_attacks[sq] & ~b->occ[b->color_to_move];
}

bb gen_n_moves_bb(int sq, struct board* b) {
	return knight_attacks[sq] & ~b->occ[b->color_to_move];
}

bb gen_p_moves_bb(int sq, struct board* b) {
	bb occ = b->occ[0] | b->occ[1];
	bb att = pawn_attacks[b->color_to_move][sq] & b->occ[!b->color_to_move];

	if (b->color_to_move != sWHITE) { // upper player
		att |= (one(sq) << 8) & ~occ;
		if ((one(sq) & pawn_start) && !((one(sq) << 8 | one(sq) << 16) & occ))
			att |= one(sq) << 16;
	} else { // lower player
		att |= (one(sq) >> 8) & ~occ;
		if ((one(sq) & pawn_start) && !((one(sq) >> 8 | one(sq) >> 16) & occ))
			att |= one(sq) >> 16;
	}
	return att;
}

bb only_capture_bb(bb b, struct board* brd) {
	return b & brd->occ[!brd->color_to_move];
}

bb only_quiet_bb(bb b, struct board* brd) {
	return b & ~brd->occ[!brd->color_to_move];
}

bb (*gen_moves_bb[6]) (int sq, struct board* b) = { gen_p_moves_bb, gen_n_moves_bb, gen_b_moves_bb, gen_r_moves_bb, gen_q_moves_bb, gen_k_moves_bb };

bool is_square_attacked(struct board* b, int pos) {
	for (int i = 0; i < 6; i++)
		if (gen_moves_bb[i](pos, b) & b->pieces[!b->color_to_move][i])
			return true;
	return false;
}

bool leaves_in_check(struct board* b, struct move* m) {
	make_move(b, m);

	int my_king = ctz(b->pieces[!b->color_to_move][pKING]); // ctz converts bb with popcount of 1 to square index
	b->color_to_move = !b->color_to_move;

	bool a = is_square_attacked(b, my_king);

	b->color_to_move = !b->color_to_move;
	unmake_move(b, m);
	return a;
}

void bb2moves(int src, bb b, struct board* board, enum piece mp, enum piece cap, enum move_flags f, struct move** m) {
	if (!b)
		return;

	for (int i = ctz(b); i < (64 - clz(b)); i++) {
		if (!(one(i) & b))
			continue;

		struct move mov = (struct move){src, i, mp, cap, f};

		if ((mov.moving_piece == pPAWN) && (one(mov.dst) & promote_spots)) {
			mov.flags |= mfPROMOTE;
		}

		if (!leaves_in_check(board, &mov)) {
			*GARY_PUSH(*m) = mov;
		}
	}
}

void gen_legal_moves(struct board* b, struct move** m) {
	for (int p = 0; p < 6; p++) { // for all piece types
		bb pbb = b->pieces[b->color_to_move][p];

		int step = 1;
		if (popcount(pbb) == 2) // skip empty squares between 2 pieces
			step = (64 - clz(pbb)) - ctz(pbb) - 1;

		if (pbb) {
			for (int i = ctz(pbb); i < (64 - clz(pbb)); i+=step) {
				if (one(i) & pbb) {
					bb movebb = gen_moves_bb[p](i, b);

					bb2moves(i, movebb & b->pieces[!b->color_to_move][pBISHOP], b, p, pBISHOP, 0, m); // bishop captures
					bb2moves(i, movebb & b->pieces[!b->color_to_move][pROOK  ], b, p, pROOK  , 0, m); // rook captures
					bb2moves(i, movebb & b->pieces[!b->color_to_move][pQUEEN ], b, p, pQUEEN , 0, m); // ...
					bb2moves(i, movebb & b->pieces[!b->color_to_move][pKING  ], b, p, pKING  , 0, m);
					bb2moves(i, movebb & b->pieces[!b->color_to_move][pKNIGHT], b, p, pKNIGHT, 0, m);
					bb2moves(i, movebb & b->pieces[!b->color_to_move][pPAWN  ], b, p, pPAWN  , 0, m);
					bb2moves(i, only_quiet_bb(movebb, b)                      , b, p, pNONE  , 0, m); // not capturing moves
				}
			}
		}
	}

	// castling
	if (b->color_to_move == sWHITE) {
		if (b->caste_white_left) { // king and rook haven't moved
			if (!((b->occ[!b->color_to_move]|b->occ[b->color_to_move]) & one(59))) { // the between squares are empty
				if (
						!is_square_attacked(b, 60) &&
						!is_square_attacked(b, 59) &&
						!is_square_attacked(b, 58)
				   ) { // the squares are not attacked
					*GARY_PUSH(*m) = (struct move) { 60, 58, pKING, pNONE, mfCASTLE };
				}
			}
		}

		if (b->caste_white_right) {
			if (!((b->occ[!b->color_to_move]|b->occ[b->color_to_move]) & one(61))) {
				if (
						!is_square_attacked(b, 60) &&
						!is_square_attacked(b, 61) &&
						!is_square_attacked(b, 62)
				   ) { // the squares are not attacked
					*GARY_PUSH(*m) = (struct move) { 60, 62, pKING, pNONE, mfCASTLE };
				}
			}
		}
	} else {
		if (b->caste_black_left) {
			if (!((b->occ[!b->color_to_move]|b->occ[b->color_to_move]) & one(3))) {
				if (
						!is_square_attacked(b, 2) &&
						!is_square_attacked(b, 3) &&
						!is_square_attacked(b, 4)
				   ) { // the squares are not attacked
					*GARY_PUSH(*m) = (struct move) { 4, 2, pKING, pNONE, mfCASTLE };
				}
			}
		}

		if (b->caste_black_right) {
			if (!((b->occ[!b->color_to_move]|b->occ[b->color_to_move]) & one(5))) {
				if (
						!is_square_attacked(b, 4) &&
						!is_square_attacked(b, 5) &&
						!is_square_attacked(b, 6)
				   ) { // the squares are not attacked
					*GARY_PUSH(*m) = (struct move) { 4, 6, pKING, pNONE, mfCASTLE };
				}
			}
		}
	}
}

