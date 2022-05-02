#include "common.h"

#include "board.h"

extern char* piece_spelling[];

enum piece char2piece(char c);
void bb2char(bb bb, char board[8][8], char c);
void board_load_fen(struct board* b, const char* fen);
void print_bb_line(bb board, int line);
void board_print(struct board* b);
void print_bb(bb board);
void print_move(struct move* m);

