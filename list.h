// vim:filetype=c
#include "common.h"
List* linit(unsigned int num, Move* val[num]);
void lfree(List* l);
void lappend(List* l, Move* val);
void lpop(List* l);
void linitpool(void);
ListNode* lgetnode(void);
void lreturnnode(ListNode* node);
