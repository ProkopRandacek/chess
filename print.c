static const char icons[] = { 'p', 'n', 'b', 'r', 'q', 'k', 'P', 'N', 'B', 'R', 'Q', 'K' };

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

void bb2char(u64 bb, char board[8][8], char c) {
	for (int i = ctz(bb); i < (64 - clz(bb)); i++)
		if (bb & ones(i))
			board[i / 8][i % 8] = c;
}

void printboard(Board* b, u64 hl) {
	char board[8][8] = {{' '}};
	memset(&board, ' ', 8 * 8 * sizeof(char));

	for (int c = 0; c < 2; c++)
		for (int p = 0; p < 6; p++)
			bb2char(b->pieces[c][p], board, icons[c * 6 + p]);

	printf("  A B C D E F G H\n");
	for (int y = 0; y < 8; y++) {
		printf("%d", 8 - y);
		for (int x = 0; x < 8; x++) {
			if ((x + y) % 2 != 0) printf(BG_BBLK);
			else printf(BG_BWHT);
			if (mask(x, y) & hl) printf(FG_GRN);
			else printf(FG_BLK);
			printf(" %c%s", board[y][x], RST);
		}
		printf("%d\n", 8 - y);
	}
	printf("  A B C D E F G H\n");
}

