#pragma once

#include "common.h"

#include "board.h"

int search(struct board* s, int a, int b, int d);
void perft(struct board* b, int d);

