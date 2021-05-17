// vim:filetype=c
#include "common.h"
int eval(Board* b, u32 moveCount);
int evalCenter(Board* b);
int pieceDiff(Board* b, int piece);
int evalPieces(Board* b);
int evalMoves(u32 moveCount);
