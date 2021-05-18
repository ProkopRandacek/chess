#include "movegen.h"

#include "board.h"
#include "debug.h"
#include "list.h"
#include "util.h"

u64 getRayBB(u64 occ, u8 dir, u8 pos) {
	u64 att = rayAttacks[dir][pos];
	u64 blockers = att & occ;
	if (blockers) {
		int c = bitScan(blockers, dir >= 6 || dir <= 1);
		att = att ^ rayAttacks[dir][c];
	}
	return att;
}

u64 genBMovesBB(u8 sq, Board* b) { u64 occ = b->occ[0] | b->occ[1]; return (getRayBB(occ, 1, sq) | getRayBB(occ, 3, sq) | getRayBB(occ, 5, sq) | getRayBB(occ, 7, sq)) & ~b->occ[b->color]; }
u64 genRMovesBB(u8 sq, Board* b) { u64 occ = b->occ[0] | b->occ[1]; return (getRayBB(occ, 0, sq) | getRayBB(occ, 2, sq) | getRayBB(occ, 4, sq) | getRayBB(occ, 6, sq)) & ~b->occ[b->color]; }
u64 genQMovesBB(u8 sq, Board* b) { u64 occ = b->occ[0] | b->occ[1]; return (getRayBB(occ, 0, sq) | getRayBB(occ, 1, sq) | getRayBB(occ, 2, sq) | getRayBB(occ, 3, sq) | getRayBB(occ, 4, sq) | getRayBB(occ, 5, sq) | getRayBB(occ, 6, sq) | getRayBB(occ, 7, sq)) & ~b->occ[b->color]; }
u64 genKMovesBB(u8 sq, Board* b) { return kingAttacks  [sq] & ~b->occ[b->color]; }
u64 genNMovesBB(u8 sq, Board* b) { return knightAttacks[sq] & ~b->occ[b->color]; }
u64 genPMovesBB(u8 sq, Board* b) {
	u64 occ = b->occ[0] | b->occ[1];
	u64 att = pawnAttacks[b->color][sq] & b->occ[!b->color];
	if (b->color) { // upper player
		att |= (ones(sq) << 8) & ~occ;
		if ((ones(sq) & pawnStart) && !((ones(sq) << 8 | ones(sq) << 16) & occ))
			att |= ones(sq) << 16;
	} else { // lower player
		att |= (ones(sq) >> 8) & ~occ;
		if ((ones(sq) & pawnStart) && !((ones(sq) >> 8 | ones(sq) >> 16) & occ))
			att |= ones(sq) >> 16;
	}
	return att;
}
static u64 (*genMovesBB[6]) (u8 sq, Board* b) = { genPMovesBB, genNMovesBB, genBMovesBB, genRMovesBB, genQMovesBB, genKMovesBB };

void bb2moves(u8 src, u64 bb, Board* b, List* l) {
	for (u8 i = (u8)ctz(bb); i < (64 - clz(bb)); i++)
		if (ones(i) & bb) {
			Move* m = dmalloc(sizeof(Move));
			m->src = src;
			m->dst = i;
			printf("found move: %d -> %d\n", m->src, m->dst);
			if (!leavesInCheck(b, m)) // check legality right after move creation => quick quick
				lappend(l, m);
			else dfree(m);
		}
}

void genLegalMoves(Board* b, List* l) {
	for (int p = 0; p < 6; p++) { // for all piece types
		u64 pbb = b->pieces[b->color][p];
		printf("pbb:\n");
		bbprint(pbb);
		for (u8 i = (u8)ctz(pbb); i < (64 - clz(pbb)); i++) // iterate over them
			if (ones(i) & pbb) // skip empty tiles
				bb2moves(i, genMovesBB[p](i, b), b, l); // append their moves to the list
	}
}

bool leavesInCheck(Board* old, Move* m) {
	Board b;
	applyMove(&b, old, m);
	b.color = !b.color; // switch color
	u8 myKing = (u8)ctz(b.pieces[b.color][KING]); // ctz converts bitboard with popcount of 1 to square index
	bool heclr = !b.color;
	u64 hepiece = b.pieces[heclr][PAWN];
	if (pawnAttacks[b.color][myKing] & hepiece) return true;
	for (int i = 1; i < 6; i++) {
		hepiece = b.pieces[heclr][i];
		if (genMovesBB[i](myKing, &b) & hepiece) return true;
	}
	return false;
}

