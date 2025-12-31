#include "editor.h"
#include "tilemap.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define NK_IMPLEMENTATION
#include "nk_raylib.h"

void run_editor()
{
    editor_state_t s = {0};
    s.window_w = 1280;
    s.window_h = 720;

    s.tile_w = 32;
    s.tile_h = 32;

    int edit_area_margin = s.tile_w * 2;
    s.edit_area.left = 250;
    s.edit_area.right = 250;
    s.edit_area.top = 40;
    s.edit_area.bottom = 200;
    s.edit_area.border_thickness = 2;
    
	InitWindow(s.window_w, s.window_h, "level-editor");

	s.bg_color.r = 60;
    s.bg_color.g = 60;
    s.bg_color.b = 60;
    s.bg_color.a = 255;

	s.font = LoadFontEx(ASSETS_PATH"ProggyClean.ttf", 24, NULL, 0);
	s.nk_font = nk_raylib_create_user_font(&(s.font));

    tilemap_create(&(s.tilemap), s.window_w / s.tile_w, s.window_h / s.tile_h);

    s.cursor_x = 0;
    s.cursor_y = 0;

    struct nk_context* ctx = &(s.nk_ctx);
	nk_init_default(ctx, &(s.nk_font));

	while (WindowShouldClose() == 0)
	{
		nk_input_begin(ctx);

		Vector2 mouse_pos = GetMousePosition();
		nk_input_motion(ctx, (int)(mouse_pos.x), (int)(mouse_pos.y));

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			nk_input_button(ctx, NK_BUTTON_LEFT,
            (int)(mouse_pos.x), (int)(mouse_pos.y), true);
		}
		else if (IsMouseButtonUp(MOUSE_BUTTON_LEFT))
		{
			nk_input_button(ctx, NK_BUTTON_LEFT,
            (int)(mouse_pos.x), (int)(mouse_pos.y), false);
		}

        if (IsKeyPressed(KEY_D))
        {
            s.cursor_x += 1;
        }
        else if (IsKeyPressed(KEY_A))
        {
            s.cursor_x -= 1;
        }

        if (IsKeyPressed(KEY_W))
        {
            s.cursor_y -= 1;
        }
        else if (IsKeyPressed(KEY_S))
        {
            s.cursor_y += 1;
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            int id = tilemap_get(&s.tilemap, s.cursor_x, s.cursor_y);
            if (id == TILE_EMPTY)
            {
                tilemap_set(&s.tilemap, s.cursor_x, s.cursor_y, 1);
            }
            else if (id != TILE_INVALID)
            {
                tilemap_set(&s.tilemap, s.cursor_x, s.cursor_y, TILE_EMPTY);
            }
        }

		BeginDrawing();
		ClearBackground(s.bg_color);

        Rectangle edit_area_rect = {
            0 + s.edit_area.left - s.edit_area.border_thickness,
            0 + s.edit_area.top - s.edit_area.border_thickness,
            s.window_w - s.edit_area.right - s.edit_area.left + s.edit_area.border_thickness,
            s.window_h - s.edit_area.bottom - s.edit_area.top + s.edit_area.border_thickness
        };

        DrawRectangleLinesEx(edit_area_rect, 6, BLACK);

        for (int tile_y = 0; tile_y < s.tilemap.height; tile_y++)
        {
            for (int tile_x = 0; tile_x < s.tilemap.width; tile_x++)
            {
                int x = tile_x * s.tile_w;
                int y = tile_y * s.tile_h;

/*                if (x < edit_area_rect.x) continue;
                if (y < edit_area_rect.y) continue;
                if ((x + s.tile_w) > (edit_area_rect.x + edit_area_rect.width))
                {
                    continue;
                }
                if ((y + s.tile_h) > (edit_area_rect.y + edit_area_rect.height))
                {
                    continue;
                }*/

                if (tilemap_get(&(s.tilemap), tile_x, tile_y) != TILE_EMPTY)
                {
                    DrawRectangle(x, y, s.tile_w, s.tile_h,
                    GREEN);
                }

                Rectangle outline = {
                    (float)x, (float)y,
                    (float)s.tile_w, (float)s.tile_h
                };
                Color outline_color = {
                    255, 255, 255, 20
                };
                DrawRectangleLinesEx(outline, 1, outline_color);
            }
        }

        DrawRectangle(0, 0, edit_area_rect.x, s.window_h, s.bg_color);
        DrawRectangle(0, 0, s.window_w, edit_area_rect.y, s.bg_color);
        DrawRectangle(edit_area_rect.x + edit_area_rect.width, 0,
                    s.window_w - edit_area_rect.x, s.window_h,
                    s.bg_color);

        DrawRectangle(0, edit_area_rect.y + edit_area_rect.height,
                    s.window_w, s.window_h - edit_area_rect.height,
                    s.bg_color);

        Rectangle cursor_rect = {
            (float)(s.cursor_x * s.tile_w), (float)(s.cursor_y * s.tile_h),
            (float)s.tile_w, (float)s.tile_h
        };
        DrawRectangleLinesEx(cursor_rect, 4, BLUE);

		nk_raylib_draw_commands(ctx);

		EndDrawing();

		nk_clear(ctx);
	}

    tilemap_delete(&(s.tilemap));
	UnloadFont(s.font);

	nk_free(ctx);
	CloseWindow();
}