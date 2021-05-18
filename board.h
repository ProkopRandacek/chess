// vim:filetype=c
#include "common.h"
void loadFen(Board* b, const char* fen);
void refreshOcc(Board* b);
u32 pieceOn(Board* b, int pos, u32 color);
void clearPos(Board* b, int pos);
void applyMove(Board* b, Board* old, Move* m);
int safePieceOn(Board* b, int pos);
