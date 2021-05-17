//vim:filetype=c

#include "common.h"

int max(int a, int b);
int min(int a, int b);
u32 char2piece(char c);
u64 getMy(Board* b, int plr);
u8 bitScan(u64 bb, bool reverse);
bool lcontains(List* l, Move* m);
void readline(char* line);
int moveGenTest(Board* b, int d);

#ifdef DEBUG
void* dmalloc(size_t sz);
void  dfree(void* mem);
void  checkatend(void);
#else
#define dmalloc malloc
#define dfree free
#endif

