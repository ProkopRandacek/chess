#include <raylib.h>
#include "main.h"
#include "list.h"
#include "util.h"
#include "movegen.h"
#include "print.h"

#include "board.h"

enum sizes {
	TILE_SIZE = 100,
	BOARD_X_PADDING = 400,
	BOARD_Y_PADDING = 100,
	SCREEN_WIDTH  = TILE_SIZE * 8 + BOARD_X_PADDING * 2,
	SCREEN_HEIGHT = TILE_SIZE * 8 + BOARD_Y_PADDING * 2,

};

const char* texNames[12] = {
	"assets/bp.png", "assets/bn.png", "assets/bb.png", "assets/br.png", "assets/bq.png", "assets/bk.png",
	"assets/wp.png", "assets/wn.png", "assets/wb.png", "assets/wr.png", "assets/wq.png", "assets/wk.png"
};
Texture2D pieces[12];
Font inconsolata;

int main(void) {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "chess");
	SetTargetFPS(60);

	for (int i = 0; i < 12; i++) {
		Image img = LoadImage(texNames[i]);
		ImageResize(&img, TILE_SIZE, TILE_SIZE);
		pieces[i] = LoadTextureFromImage(img);
	}
	inconsolata = LoadFontEx("assets/inconsolata.ttf", TILE_SIZE, 0, 250);

	startRenderLoop();

#ifdef DEBUG
	checkatend();
#endif
	return 0;
}

void bb2char(u64 bb, char board[8][8], char c) {
	for (int i = ctz(bb); i < (64 - clz(bb)); i++)
		if (bb & ones(i))
			board[i / 8][i % 8] = c;
}

void startRenderLoop(void) {
	Board board;
	loadFen(&board, "rnbqkbnr/pppppppp/////PPPPPPPP/RNBQKBNR");
	board.color = LOWER;

	bool moving = false;
	int movingStart = -1;
	int movingStop  = -1;
	int movingPiece = -1;
	Vector2 mousePos = {0, 0};
	u64 hlChange = 0;
	u64 hlPossible = 0;
	List* l;

	while (!WindowShouldClose()) {
		if (moving) {
			mousePos = GetMousePosition();
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) { // still moving

			} else { // stopped moving
				moving = false;
				movingStop = (int)(mousePos.y - BOARD_Y_PADDING) / TILE_SIZE * 8 + (int)(mousePos.x - BOARD_X_PADDING) / TILE_SIZE;
				Move move;
				move.src = (u8)movingStart;
				move.dst = (u8)movingStop;
				printf("%d -> %d\n", move.src, move.dst);
				printf("moves: %d\n", l->count);
				hlPossible = 0;
				if (!lcontains(l, &move)) {
					printf("illegal move\n");
					movingStart = -1;
					movingStop  = -1;
					movingPiece = -1;
					continue;
				}
				lfree(l);
				Board newBoard;
				applyMove(&newBoard, &board, &move);
				board = newBoard;
				hlChange = ones(move.src) | ones(move.dst);
			}
		} else {
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) { // started moving
				moving = true;
				mousePos = GetMousePosition();
				if (mousePos.x > BOARD_X_PADDING && mousePos.y > BOARD_Y_PADDING && mousePos.x < BOARD_X_PADDING + TILE_SIZE * 8 && mousePos.y < BOARD_Y_PADDING + TILE_SIZE * 8) {
					movingStart = (int)(mousePos.y - BOARD_Y_PADDING) / TILE_SIZE * 8 + (int)(mousePos.x - BOARD_X_PADDING) / TILE_SIZE;
					movingPiece = safePieceOn(&board, movingStart);
				}

				l = linit(1, (void*[]){0}); // create legal moves
				genLegalMoves(&board, l);
				lpop(l);
				hlPossible = moveEnds(l, movingStart);
			} else {
				moving = false;
			}
		}
		BeginDrawing();

		ClearBackground(GRAY);

		// chess grid
		DrawRectangle(BOARD_X_PADDING, BOARD_Y_PADDING, TILE_SIZE * 8, TILE_SIZE * 8, DARKGRAY);
		for (int y = 0; y < 8 * TILE_SIZE; y += TILE_SIZE)
			for (int x = y % (TILE_SIZE * 2); x < 8 * TILE_SIZE; x += TILE_SIZE * 2)
				DrawRectangle(BOARD_X_PADDING + x, BOARD_Y_PADDING + y, TILE_SIZE, TILE_SIZE, LIGHTGRAY);
		for (int y = 0; y < 8; y++)
			for (int x = 0; x < 8; x++) {
				if (hlPossible & mask(x, y))
					DrawRectangle(BOARD_X_PADDING + x * TILE_SIZE, BOARD_Y_PADDING + y * TILE_SIZE, TILE_SIZE, TILE_SIZE, GREEN);
				else if (hlChange & mask(x, y))
					DrawRectangle(BOARD_X_PADDING + x * TILE_SIZE, BOARD_Y_PADDING + y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLUE);
			}

		// pieces
		char charBoard[8][8];
		memset(&charBoard, 0, sizeof(char)*64);
		for (int c = 0; c < 2; c++)
			for (int p = 0; p < 6; p++)
				bb2char(board.pieces[c][p], charBoard, c * 6 + p + 1);
		for (int y = 0; y < 8; y++)
			for (int x = 0; x < 8; x++)
				if (charBoard[y][x] != 0 && movingStart != y * 8 + x)
					DrawTexture(pieces[charBoard[y][x] - 1], x * TILE_SIZE + BOARD_X_PADDING, y * TILE_SIZE + BOARD_Y_PADDING, WHITE);

		// moving piece
		if (moving) DrawTexture(pieces[movingPiece], mousePos.x - TILE_SIZE / 2, mousePos.y - TILE_SIZE / 2, WHITE);

		// texts
		DrawTextEx(inconsolata, "Mate in 6 ply", (Vector2) { 10, BOARD_Y_PADDING + TILE_SIZE * 3}, TILE_SIZE / 2, 0.5f, WHITE);
		DrawTextEx(inconsolata, "Move: e2e4"   , (Vector2) { 10, BOARD_Y_PADDING + TILE_SIZE * 4}, TILE_SIZE / 2, 0.5f, WHITE);

		EndDrawing();
	}
	for (int i = 0; i < 12; i++)
		UnloadTexture(pieces[i]);
	CloseWindow();
}

