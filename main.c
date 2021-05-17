#include "common.h"
#include "board.h"
#include "gui.h"

int main(void) {
	Board board;
	loadFen(&board, "rnbqkbnr/pppppppp/////PPPPPPPP/RNBQKBNR");
	board.color = LOWER;

	guiInit();
	startRenderLoop();
#ifdef DEBUG
	checkatend();
#endif
	return 0;
}

