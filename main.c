#include <raylib.h>
#include "main.h"
#include "list.h"
#include "util.h"
#include "movegen.h"
#include "print.h"
#include "search.h"

#include "board.h"

Move  emptyMove = (Move){0, 0};
Move* emptyMoveList[1] = { &emptyMove };

extern u64 searchCount;

enum sizes {
	TILE_SIZE = 100,
	BOARD_X_PADDING = 400,
	BOARD_Y_PADDING = 100,
	SCREEN_WIDTH  = TILE_SIZE * 8 + BOARD_X_PADDING * 2,
	SCREEN_HEIGHT = TILE_SIZE * 8 + BOARD_Y_PADDING * 2,
};

const char* texNames[12] = { "assets/bp.png", "assets/bn.png", "assets/bb.png", "assets/br.png", "assets/bq.png", "assets/bk.png", "assets/wp.png", "assets/wn.png", "assets/wb.png", "assets/wr.png", "assets/wq.png", "assets/wk.png" };
Texture2D pieces[12];
Font font;

int main(void) {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "chess");
	SetTargetFPS(60);

	for (int i = 0; i < 12; i++) {
		Image img = LoadImage(texNames[i]);
		ImageResize(&img, TILE_SIZE, TILE_SIZE);
		pieces[i] = LoadTextureFromImage(img);
	}
	font = LoadFontEx("assets/inconsolata.ttf", TILE_SIZE, 0, 250);

	startRenderLoop();

#ifdef DEBUG
	checkatend();
#endif
	return 0;
}

void startRenderLoop(void) {
	Board board;
	//loadFen(&board, "rnbqkbnr/pppppppp/////PPPPPPPP/RNBQKBNR");
	loadFen(&board, "r3k2r/p1ppqpb/bn2pnp/3PN/1p2P/2N2Q1p/PPPBBPPP/R3K2R");
	board.color = LOWER;

	bool moving      = false; // is user moving with a piece?
	int  movingStart = 64;    // where did he get that piece that he is moving with?
	int  movingStop  = 64;    // where did he put it?
	int  movingPiece = 64;    // what piece it was?
	u64  hlChange    = 0;     // what piece moved since last time?
	u64  hlPossible  = 0;     // what are the possible moves with this piece that hes holding?

	Vector2 mousePos = {0, 0}; // where is mouse?
	List* l; // list of moves

	//char* lines[3] = { "possible moves: 33", "depth search: 11", "idk stat 3" };

	while (!WindowShouldClose()) {
		if (moving) {
			mousePos = GetMousePosition();
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) { // still moving
				// idk do nothing
			} else { // stopped moving
				moving = false;
				movingStop = (((int)mousePos.y - BOARD_Y_PADDING) / TILE_SIZE * 8) + (((int)mousePos.x - BOARD_X_PADDING) / TILE_SIZE);
				Move move = (Move){ (u8)movingStart, (u8)movingStop };
				printf("your move: %d -> %d\n", move.src, move.dst);
				hlPossible = 0;
				if (!lcontains(l, &move)) { // check if move is legal
					printf("illegal move\n");
					movingStart = 64;
					movingStop  = 64;
					movingPiece = 64;
					lfree(l);
					continue;
				}
				lfree(l); // TODO: free it while checking if it contains that move
				{ // human move
					Board newBoard; // apply move to the board
					applyMove(&newBoard, &board, &move);
					board = newBoard;
					hlChange = ones(move.src) | ones(move.dst);
				}
				{ // AI move
					Board newBoard;
					Move aiMove;
					makeAIMove(&aiMove, &board, 4);
					applyMove(&newBoard, &board, &aiMove);
					board = newBoard;
					hlChange = ones(aiMove.src) | ones(aiMove.dst);
				}
			}
		} else {
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) { // started moving
				moving = true;
				mousePos = GetMousePosition();
				if (mousePos.x > BOARD_X_PADDING && mousePos.y > BOARD_Y_PADDING && mousePos.x < BOARD_X_PADDING + TILE_SIZE * 8 && mousePos.y < BOARD_Y_PADDING + TILE_SIZE * 8) {
					movingStart = (int)(mousePos.y - BOARD_Y_PADDING) / TILE_SIZE * 8 + (int)(mousePos.x - BOARD_X_PADDING) / TILE_SIZE;
					movingPiece = safePieceOn(&board, movingStart);
				}

				l = linit(1, emptyMoveList); // create legal moves
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
			for (int x = 0; x < 8; x++)
				if (hlPossible & mask(x, y))
					DrawRectangle(BOARD_X_PADDING + x * TILE_SIZE, BOARD_Y_PADDING + y * TILE_SIZE, TILE_SIZE, TILE_SIZE, GREEN);
				else if (hlChange & mask(x, y))
					DrawRectangle(BOARD_X_PADDING + x * TILE_SIZE, BOARD_Y_PADDING + y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLUE);

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
		if (moving) DrawTexture(pieces[movingPiece], (int)mousePos.x - TILE_SIZE / 2, (int)mousePos.y - TILE_SIZE / 2, WHITE);

		// texts
		//DrawTextEx(font, "This is line 1", (Vector2) { 10, BOARD_Y_PADDING + TILE_SIZE * 3}, TILE_SIZE / 2, 0.5f, WHITE);
		//DrawTextEx(font, "line 2"        , (Vector2) { 10, BOARD_Y_PADDING + TILE_SIZE * 4}, TILE_SIZE / 2, 0.5f, WHITE);
		//DrawTextEx(font, "3333333333"    , (Vector2) { 10, BOARD_Y_PADDING + TILE_SIZE * 5}, TILE_SIZE / 2, 0.5f, WHITE);

		EndDrawing();
	}
	for (int i = 0; i < 12; i++) UnloadTexture(pieces[i]);
	CloseWindow();
	printf("searchcount: %ld\n", searchCount);
}

