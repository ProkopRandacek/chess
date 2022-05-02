#pragma once
#include "common.h"

extern const int piece_score[5];
extern const int checkmate_score;
extern const int center_score;
extern const int move_score;
extern const bb center;
extern const bb pawn_start;
extern const bb promote_spots;

extern const bb ray_attacks[8][64];
extern const bb king_attacks[64];
extern const bb knight_attacks[64];
extern const bb pawn_attacks[2][64];
