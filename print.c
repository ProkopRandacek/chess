#include "print.h"

void bbprint(u64 bb) {
	u64 mask = 1;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			char c = mask & bb ? '1' : '0';
			printf(" %c", c);
			mask = mask << 1;
		}
		printf("\n");
	}
}


