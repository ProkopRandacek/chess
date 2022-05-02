#pragma once

#include "common.h"

enum move_flags {
	mfCASTLE  = 0b00001,
	mfENPASS  = 0b00010,
	mfPROMOTE = 0b00100,
};

struct move {
	int src;
	int dst;
	enum piece moving_piece;
	enum piece capturing_piece; // can be none
	enum move_flags flags;
};
