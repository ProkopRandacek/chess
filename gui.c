#include <raylib.h>

enum sizes {
	TILE_SIZE = 100,
	BOARD_X_PADDING = 300,
	BOARD_Y_PADDING = 100,
	SCREEN_WIDTH  = TILE_SIZE * 8 + BOARD_X_PADDING * 2,
	SCREEN_HEIGHT = TILE_SIZE * 8 + BOARD_Y_PADDING * 2,

};

const char* texNames[12] = {
	"assets/wp.png", "assets/wn.png", "assets/wb.png", "assets/wr.png", "assets/wq.png", "assets/wk.png",
	"assets/bp.png", "assets/bn.png", "assets/bb.png", "assets/br.png", "assets/bq.png", "assets/bk.png",
};

int guiBoard[64] = { };
Texture2D pieces[12];

void setPiece(int x, int y, int piece) { guiBoard[y * 8 + x] = piece; }

void guiInit(void) {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "chess");
	SetTargetFPS(60);

	for (int i = 0; i < 12; i++) {
		Image img = LoadImage(texNames[i]);
		ImageResize(&img, TILE_SIZE, TILE_SIZE);
		pieces[i] = LoadTextureFromImage(img);
	}
}

void startRenderLoop(void) {
	// Main game loop
	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(RAYWHITE);

		// chess grid
		DrawRectangle(BOARD_X_PADDING, BOARD_Y_PADDING, TILE_SIZE * 8, TILE_SIZE * 8, GREEN);
		for (int y = 0; y < 8 * TILE_SIZE; y += TILE_SIZE)
			for (int x = y % (TILE_SIZE * 2); x < 8 * TILE_SIZE; x += TILE_SIZE * 2)
				DrawRectangle(BOARD_X_PADDING + x, BOARD_Y_PADDING + y, TILE_SIZE, TILE_SIZE, BLUE);

		// pieces
		for (int i; i < 64; i++) {
			DrawTexture(pieces[1], 0, 0, WHITE);
		}

		EndDrawing();
	}
	CloseWindow();
}

