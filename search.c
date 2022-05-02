#include "search.h"

#include "board.h"
#include "eval.h"
#include "movegen.h"
#include "table.h"

#include <ucw/lib.h>
#include <ucw/gary.h>

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

