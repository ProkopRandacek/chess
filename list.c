List* linit(unsigned int num, list_val_t val[num]) {
	if (num <= 0) {
		printf("%s - list length must be greater than zero (is %d)\n", __func__, num);
		exit(1);
	}
	List*     l = dmalloc(sizeof(List));
	ListNode* n = dmalloc(sizeof(ListNode));
	n->val   = val[0];
	l->first = n;
	l->last  = n;
	l->count = 1;
	for (unsigned int i = 1; i < num; i++)
		lappend(l, val[i]);
	return l;
}

void lfree(List* l) {
	ListNode* pos = l->first;
	for (;;) {
		ListNode* tmp = pos->next;
		dfree(pos->val);
		dfree(pos);
		if (tmp == 0) break;
		pos = tmp;
	}
	dfree(l);
}

void lappend(List* l, list_val_t val) {
	l->last->next = dmalloc(sizeof(ListNode)); // create new node at the end
	l->last->next->val = val; // paste the value
	l->last = l->last->next; // move the end pointer
	l->count++;
}

void lpop(List* l) {
	ListNode* pop = l->first;
	l->first = l->first->next;
	l->count--;
	dfree(pop->val);
	dfree(pop);
}

