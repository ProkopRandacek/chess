#ifdef DEBUG
unsigned int mallocs = 0;
unsigned int frees = 0;
size_t total = 0;

void* dmalloc(size_t sz) {
	void* mem = malloc(sz);
	total += sz;
	mallocs++;
	//printf("malloc: %p\n", mem);
	return mem;
}
void dfree(void* mem) {
	frees++;
	//printf("free:   %p\n", mem);
	free(mem);
}
void checkatend() {
	printf("frees:   %d\nmallocs: %d\ntotal:   %ld bytes\n", frees, mallocs, total);
}
#endif
