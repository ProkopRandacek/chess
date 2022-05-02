#pragma once

#include "common.h"

#include "board.h"

int eval(struct board* b, int moveCount);
int evalCenter(struct board* b);
int pieceDiff(struct board* b, int piece);
int evalPieces(struct board* b);
int evalMoves(int moveCount);
