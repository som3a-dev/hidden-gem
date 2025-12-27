#include <stdlib.h>
#include <stdio.h>

#define NK_IMPLEMENTATION
#include "nk_raylib.h"

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

static int window_width = 1280;
static int window_height = 720;

static void init_crtdbg();
static void dump_crtdbg();

int main(void)
{
	init_crtdbg();
	InitWindow(window_width, window_height, "level-editor");
	printf("%s\n", ASSETS_PATH);

	Color bg_color = {60, 60, 60, 255};

	Font font = LoadFontEx(ASSETS_PATH"ProggyClean.ttf", 24, NULL, 0);
	struct nk_user_font nk_font = nk_raylib_create_user_font(&font);
	struct nk_context ctx;

	nk_init_default(&ctx, &nk_font);

	while (WindowShouldClose() == 0)
	{
		nk_input_begin(&ctx);

		Vector2 mouse_pos = GetMousePosition();
		nk_input_motion(&ctx, (int)(mouse_pos.x), (int)(mouse_pos.y));

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			nk_input_button(&ctx, NK_BUTTON_LEFT, (int)(mouse_pos.x), (int)(mouse_pos.y), true);
		}
		else if (IsMouseButtonUp(MOUSE_BUTTON_LEFT))
		{
			nk_input_button(&ctx, NK_BUTTON_LEFT, (int)(mouse_pos.x), (int)(mouse_pos.y), false);
		}

		nk_input_end(&ctx);

		if (nk_begin(&ctx, "level-editor", nk_rect(0, 0, (float)window_width, (float)window_height),
			NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
			// fixed widget pixel width
			nk_layout_row_static(&ctx, 0, (int)(MeasureText("button", font.baseSize) * 1.5f), 1);
			if (nk_button_label(&ctx, "button")) {
				// event handling
				printf("Pressed\n");
			}
		}
		nk_end(&ctx);

		BeginDrawing();
		ClearBackground(bg_color);

		nk_raylib_draw_commands(&ctx);

		EndDrawing();

		nk_clear(&ctx);
	}

	nk_free(&ctx);
	UnloadFont(font);
	CloseWindow();

	dump_crtdbg();
	return 0;
}

static void init_crtdbg()
{
    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}

static void dump_crtdbg()
{
	_CrtDumpMemoryLeaks();
}
