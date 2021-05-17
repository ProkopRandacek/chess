#include "move.h"

#include "util.h"

Move* moveFromString(char* string) {
	int srcx =   (string[0] - 97);
	int srcy = 8-(string[1] - 48);
	// space
	int dstx =   (string[3] - 97);
	int dsty = 8-(string[4] - 48);
	Move* m = dmalloc(sizeof(Move));
	m->src = (u8)(srcy * 8 + srcx);
	m->dst = (u8)(dsty * 8 + dstx);
	//printf("%s = %d, %d -> %d, %d (%d -> %d)\n", string, srcx, srcy, dstx, dsty, m->src, m->dst);
	return m;
}

