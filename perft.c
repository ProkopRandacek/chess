#include "perft.h"

#include "board.h"
#include "movegen.h"
#include <ucw/lib.h>
#include <ucw/gary.h>

static int _perft(struct board* b, int d) {
	if (d == 0) {
		return 1;
	}

	struct move* moves;
	GARY_INIT(moves, 0);
	gen_legal_moves(b, &moves);

	size_t moves_count = GARY_SIZE(moves);

	size_t sum = 0;

	for (size_t pos = 0; pos < moves_count; pos++) {
		make_move(b, &moves[pos]);
		sum += _perft(b, d-1);
		unmake_move(b, &moves[pos]);
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
		printf("%c%d%c%d - %d\n", (m->src%8)+'a', (7-m->src/8)+1, (m->dst%8)+'a', (7-m->dst/8)+1, sum);
		unmake_move(b, m);
	}
}

