#pragma once

#include "common.h"

#include "board.h"

bb get_ray_bb(bb occ, int dir, int pos);

bb gen_b_moves_bb(int sq, struct board* b);
bb gen_r_moves_bb(int sq, struct board* b);
bb gen_q_moves_bb(int sq, struct board* b);
bb gen_k_moves_bb(int sq, struct board* b);
bb gen_n_moves_bb(int sq, struct board* b);
bb gen_p_moves_bb(int sq, struct board* b);

void gen_legal_moves(struct board* b, struct move**);
bool leaves_in_check(struct board* b, struct move*);

void bb2moves(int src, bb b, struct board* board, enum piece mov, enum piece cap, enum move_flags f, struct move** m);

bb only_capture_bb(bb b, struct board* brd);
bb only_quiet_bb(bb b, struct board* brd);
