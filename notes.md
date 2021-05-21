# About the Linked List
The only value that lists can hold is `Move`.  

## List init
List initialization is done like this:
```c
Move  emptyMove = (Move){0, 0};
Move* emptyMoveList[1] = { &emptyMove };
List* l = linit(1, emptyMoveList); // create list with one node `emptyMove`
// append the list at least once
lpop(l); // pop the empty move node.
// now you can iterate over the list or something
```
The `emptyMoveList` variable is globally defined in `main.c` and used every time new list needs to be created.  
You cannot initialize empty list since that would require some `if`s in list functions.  

## List append
List append is really quick. Since `Move` is inside the `ListNode` struct, single malloc can allocate entire node.

## List freeing
List freeing is possible to do by `lfree(l)` but usually can be freed manually while iterating which is faster.
