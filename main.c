#include <stdio.h>

#include "board.h"
#include "movegen.h"
#include "search.h"
#include "gui.h"
#include "strmanip.h"
#include "perft.h"

#include <ucw/lib.h>
#include <ucw/gary.h>

int main() {
	struct board b;
	board_load_fen(&b, "rnbqkbnr/pppppppp/////PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	//load_fen(&b, "4k/8/8/8/3q/6K/8/8 w KQkq - 0 1");
	board_print(&b);

#if 0
	gui(&b);
#else
	perft(&b, 4);
#endif
}

