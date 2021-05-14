bool checkMoveString(char* string) {
	regex_t regex;
	int value;
	printf("checking: %s\n", string);
	value = regcomp(&regex, "([a-h][1-8] [a-h][1-8])", 0); // TODO compile regex at start not every time. Maybe with `static`
	if (value) {
		printf("%s - regex compilation failed\n", __func__);
		exit(1);
	}
	value = regexec(&regex, string, 0, NULL, 0); // FIXME
	if      (value == 0)           { return true;  }
	else if (value == REG_NOMATCH) { return false; }
	else {
		printf("%s - regex match failed for string %s\n", __func__, string);
		exit(1);
	}
}

Move* moveFromString(char* string) {
	int srcx =   (string[0] - 97);
	int srcy = 8-(string[1] - 48);
	// space
	int dstx =   (string[3] - 97);
	int dsty = 8-(string[4] - 48);
	// promo
	Move* m = dmalloc(sizeof(Move));
	m->src = (u8)(srcy * 8 + srcx);
	m->dst = (u8)(dsty * 8 + dstx);
	//printf("%s = %d, %d -> %d, %d (%d -> %d)\n", string, srcx, srcy, dstx, dsty, m->src, m->dst);
	m->promo = 0;
	return m;
}
