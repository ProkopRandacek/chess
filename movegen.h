// vim:filetype=c
#include "common.h"
u64 getRayBB(u64 occ, u8 dir, u8 pos);
u64 genBMovesBB(u8 sq, Board* b);
u64 genRMovesBB(u8 sq, Board* b);
u64 genQMovesBB(u8 sq, Board* b);
u64 genKMovesBB(u8 sq, Board* b);
u64 genNMovesBB(u8 sq, Board* b);
u64 genPMovesBB(u8 sq, Board* b);
void bb2moves(u8 src, u64 bb, Board* b, List* l);
void genLegalMoves(Board* b, List* l);
bool leavesInCheck(Board* old, Move* m);
