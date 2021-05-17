// vim:filetype=c
#include "common.h"
List* linit(unsigned int num, list_val_t val[num]);
void lfree(List* l);
void lappend(List* l, list_val_t val);
void lpop(List* l);
