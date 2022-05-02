#include <ucw/lib.h>

#include <raylib.h>

#include "gui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "board.h"
#include "movegen.h"
#include "strmanip.h"

#include <ucw/lib.h>
#include <ucw/gary.h>

enum sizes {
	TILE_SIZE = 100,
	BOARD_SIZE = TILE_SIZE * 8,
	BOARD_X_PADDING = 400,
	BOARD_Y_PADDING = 100,
	SCREEN_WIDTH  = BOARD_SIZE + BOARD_X_PADDING * 2,
	SCREEN_HEIGHT = BOARD_SIZE + BOARD_Y_PADDING * 2,
};

static const char* tex_names[12] = { "assets/bp.png", "assets/bn.png", "assets/bb.png", "assets/br.png", "assets/bq.png", "assets/bk.png", "assets/wp.png", "assets/wn.png", "assets/wb.png", "assets/wr.png", "assets/wq.png", "assets/wk.png" };
static Texture2D pieces[12];
static Font font;

static void load_assets(void) {
	for (int i = 0; i < 12; i++) {
		Image img = LoadImage(tex_names[i]);
		ImageResize(&img, TILE_SIZE, TILE_SIZE);
		pieces[i] = LoadTextureFromImage(img);
	}
	font = LoadFontEx("assets/inconsolata.ttf", TILE_SIZE, 0, 250);
}

static void render_loop(struct board* board) {
	int hovering = -1; // the square under cursor

	bb hl_changed  = 0; // what piece moved since last time?
	bb hl_possible = 0; // what are the possible moves with this piece that he's holding?

	bool moving = false; // is user moving with a piece?
	int move_start = 0;
	int move_end = 0;
	enum piece move_piece = 0;

	Vector2 board_cursor = {0, 0}; // cursor position within the board. can be negative if the cursor is top/left of the board
	Vector2 cursor = {0, 0}; // cursor position within the window.

	while (!WindowShouldClose()) {
		cursor = GetMousePosition();
		board_cursor = (Vector2){
			(cursor.x - BOARD_X_PADDING)/BOARD_SIZE*8,
			(cursor.y - BOARD_Y_PADDING)/BOARD_SIZE*8
		};

		if (board_cursor.x >= 0.0 && board_cursor.x <= 8.0 && board_cursor.y >= 0.0 && board_cursor.y <= 8.0) {
			hovering = ((int)board_cursor.x) + ((int)board_cursor.y * 8);
			printf("hov: %d\n", hovering);
		} else {
			hovering = -1;
		}

		if (!moving && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && hovering != -1) {
			if (hovering >= 0 && board->occ[board->color_to_move] & one(hovering)) {
				// Started moving a piece
				moving = true;
				move_start = hovering;
				move_piece = piece_on(board, hovering, board->color_to_move);
				struct move* legal;
				GARY_INIT(legal, 0);
				gen_legal_moves(board, &legal);

				size_t l = GARY_SIZE(legal);

				hl_possible = 0;
				for (size_t i = 0; i < l; i++) {
					struct move* m = &legal[i];
					if (m->src == move_start)
						hl_possible |= one(m->dst);
				}
			}
		}
		if (moving && !IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			// stopped moving
			moving = false;
			hl_possible = 0;

			move_end = hovering;

			struct move* legal;
			GARY_INIT(legal, 0);
			gen_legal_moves(board, &legal);

			size_t l = GARY_SIZE(legal);

			bool ok = false;
			struct move mov;
			for (size_t i = 0; i < l; i++) {
				struct move* m = &legal[i];
				if (m->src == move_start && m->dst == move_end) {
					ok = true;
					mov = *m;
				}
			}

			if (ok) {
				printf("ok move\n");
				print_move(&mov);
				make_move(board, &mov);
			} else
				printf("not ok move\n");

			GARY_FREE(legal);

			// execute AI move
		}

		BeginDrawing(); {

			ClearBackground(GRAY);

			// Draw grid
			DrawRectangle(BOARD_X_PADDING, BOARD_Y_PADDING, BOARD_SIZE, BOARD_SIZE, DARKGRAY);
			for (int y = 0; y < 8 * TILE_SIZE; y += TILE_SIZE)
				for (int x = y % (TILE_SIZE * 2); x < BOARD_SIZE; x += TILE_SIZE * 2)
					DrawRectangle(BOARD_X_PADDING + x, BOARD_Y_PADDING + y, TILE_SIZE, TILE_SIZE, LIGHTGRAY);

			// Highlight squares
			for (int y = 0; y < 8; y++)
				for (int x = 0; x < 8; x++)
					if (hl_possible & one_sq(x, y))
						DrawRectangle(BOARD_X_PADDING + x*TILE_SIZE, BOARD_Y_PADDING + y*TILE_SIZE, TILE_SIZE, TILE_SIZE, GREEN);
					else if (hl_changed & one_sq(x, y))
						DrawRectangle(BOARD_X_PADDING + x*TILE_SIZE, BOARD_Y_PADDING + y*TILE_SIZE, TILE_SIZE, TILE_SIZE, BLUE);

			// pieces
			char char_board[8][8];
			memset(&char_board, 0, sizeof(char)*64);
			for (int c = 0; c < 2; c++)
				for (int p = 0; p < 6; p++)
					bb2char(board->pieces[c][p], char_board, 6*c + p + 1);

			for (int y = 0; y < 8; y++)
				for (int x = 0; x < 8; x++)
					if (char_board[y][x] != 0 && !(moving && (move_start == y*8+x)))
						DrawTexture(pieces[char_board[y][x] - 1], x*TILE_SIZE + BOARD_X_PADDING, y*TILE_SIZE + BOARD_Y_PADDING, WHITE);

			// moving piece
			if (moving) DrawTexture(pieces[move_piece + 6*(board->color_to_move)], (int)cursor.x - TILE_SIZE / 2, (int)cursor.y - TILE_SIZE / 2, WHITE);

		} EndDrawing();
	}

	for (int i = 0; i < 12; i++)
		UnloadTexture(pieces[i]);

	CloseWindow();
}

int gui(struct board* b) {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "chess");
	SetTargetFPS(60);

	load_assets();

	render_loop(b);

	return 0;
}

