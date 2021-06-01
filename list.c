#include "list.h"

#include "util.h"

#define nodePoolLen 512
// on average, the list should be 37 nodes long.
// there are 4 levels of recursion
// = 37 * 4 = 148 Nodes on average allocated

ListNode* nodePool; // the pool
bool poolTaken[nodePoolLen] = { false }; // list of what nodes are free and what not; false = free; true = taken
int firstEmpty = 0; // cached first empty node

/*int maxTaken = 0;
u64 totalMoved = 0;
int rnTaken = 0;
int takes = 0;
int returns = 0;*/

void linitpool() {
	nodePool = malloc(sizeof(ListNode) * nodePoolLen);
}

void lfreepool() {
	free(nodePool);
}

// note that the behaviour when we run out of pool is undefined
ListNode* lgetnode() {
	int toReturn = firstEmpty;

	// move first empty
	poolTaken[firstEmpty] = true;
	while (poolTaken[firstEmpty] == true)
		firstEmpty++;

	/*totalMoved += firstEmpty - toReturn;

	takes++;
	rnTaken++;
	maxTaken = max(maxTaken, rnTaken);*/

	// return node
	return nodePool + toReturn;
}

void lreturnnode(ListNode* node) {
	long int nodeIndex = node - nodePool;
	poolTaken[nodeIndex] = false;
	firstEmpty = min((int)nodeIndex, firstEmpty); // update first empty
	//returns++;
	//rnTaken--;
}

List* linit(unsigned int num, Move* val[num]) {
	if (num <= 0) {
		printf("%s - list length must be greater than zero (is %d)\n", __func__, num);
		exit(1);
	}
	List*     l = dmalloc(sizeof(List));
	ListNode* n = lgetnode();
	n->m     = *val[0];
	n->next  = NULL;
	l->first = n;
	l->last  = n;
	l->count = 1;
	for (unsigned int i = 1; i < num; i++)
		lappend(l, val[i]);
	return l;
}

void lfree(List* l) {
	ListNode* pos = l->first;
	while (1) {
		if (pos == NULL) break;
		if (pos->next == NULL) break;
		ListNode* tmp = pos->next;
		lreturnnode(pos);
		pos = tmp;
	}
	if (pos != 0)
		lreturnnode(pos);
	dfree(l);
}

void lappend(List* l, Move* m) {
	l->last->next = lgetnode(); // create new node at the end
	l->last->next->next = NULL;
	l->last->next->m = *m; // paste the value
	l->last = l->last->next; // move the end pointer
	l->count++;
}

void lpop(List* l) { // deletes first element
	ListNode* pop = l->first;
	l->first = l->first->next;
	l->count--;
	lreturnnode(pop);
}

