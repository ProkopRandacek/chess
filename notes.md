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
List append is really quick. `List` contains pointer to the last node. That way, appending can be done in couple of instruction.  
Since `Move` is inside the `ListNode` struct, single malloc call can allocate entire node and single free call can free it.  

## List freeing
List freeing is possible to do by `lfree(l)` but usually can be freed manually while iterating which is faster.  
`lfree` implementation is little weird.

# About check check
If king can be captured by bishop, knight, rook or queen, you can switch the attacking piece and king and king would be still in check.  
Because attacks are symmetrical in this way, you can pretend that king moves like knight, check if it can capture enemy knight and if yes,
the king is in check by an enemy knight. This way are all piece types checked and check is detected without actually generating all enemy moves.