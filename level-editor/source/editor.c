#include "asset.h"
#include "collision.h"
#include "editor.h"
#include "tilemap.h"
#include "nk_raylib.h"

#include "editor_cursor.h"
#include "editor_camera.h"
#include "editor_ui.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static void editor_init(editor_state_t* s);
static void editor_delete(editor_state_t* s);

static void editor_update(editor_state_t* s);
static void editor_update_zoom(editor_state_t* s);

static void editor_draw(editor_state_t* s);
static void editor_draw_edit_area(editor_state_t* s);
static void editor_draw_tilemap(editor_state_t* s);

static struct nk_context* ctx = NULL;

void run_editor()
{
    editor_state_t s = {0};
    editor_init(&s);

	while (WindowShouldClose() == 0)
	{
        editor_update(&s);
        editor_draw(&s);
	}

    editor_delete(&s);
}

static void editor_init(editor_state_t* s)
{
    s->window_w = 1280;
    s->window_h = 720;

	InitWindow(s->window_w, s->window_h, "level-editor");
    SetTargetFPS(60);

    asset_load_texture(ASSETS_PATH"block.png");
    asset_load_texture(ASSETS_PATH"dirt0.png");

    s->tile_w = 32;
    s->tile_h = 32;

    s->edit_area.left = 250;
    s->edit_area.right = 250;
    s->edit_area.top = 40;
    s->edit_area.bottom = 200;
    s->edit_area.border_thickness = 1;

	s->bg_color.r = 60;
    s->bg_color.g = 60;
    s->bg_color.b = 60;
    s->bg_color.a = 255;

	s->font = LoadFontEx(ASSETS_PATH"DroidSans.ttf", 24, NULL, 0);
	s->nk_font = nk_raylib_create_user_font(&(s->font));

    tileset_add_tile(&s->tileset, 1, ASSETS_PATH"dirt0.png");
    tileset_add_tile(&s->tileset, 2, ASSETS_PATH"block.png");

    s->selected_tile_id = 1;

    tilemap_create(&(s->tilemap), s->window_w / s->tile_w, s->window_h / s->tile_h);

    Rectangle edit_rect = edit_area_get_rect(&(s->edit_area), s->window_w, s->window_h);
    s->cursor_x = (int)(edit_rect.x / s->tile_w) + 1;
    s->cursor_y = (int)(edit_rect.y / s->tile_h) + 1;

    ctx = &(s->nk_ctx);
	nk_init_default(ctx, &(s->nk_font));
}

static void editor_delete(editor_state_t* s)
{
    asset_delete_assets();

    tilemap_delete(&(s->tilemap));
	UnloadFont(s->font);

	nk_free(ctx);
	CloseWindow();
}

static void editor_update(editor_state_t* s)
{
    editor_update_ui_input(s);
    editor_update_zoom(s);
    editor_update_camera(s);
    editor_update_cursor(s);
}

static void editor_update_zoom(editor_state_t* s)
{
    if (IsKeyPressed(KEY_EQUAL))
    {
        s->edit_area.border_thickness += 1;
    }
    else if (IsKeyPressed(KEY_MINUS))
    {
        s->edit_area.border_thickness -= 1;
    }

    if (s->edit_area.border_thickness < 1)
    {
        s->edit_area.border_thickness = 1;
    }
}

static void editor_draw(editor_state_t* s)
{
    BeginDrawing();
    ClearBackground(s->bg_color);

    editor_draw_tilemap(s);
    editor_draw_edit_area(s);
    if (s->draw_cursor)
    {
        editor_draw_cursor(s);
    }

    editor_ui_tileset(s);

    nk_raylib_draw_commands(ctx);

    EndDrawing();

    nk_clear(ctx);
}

static void editor_draw_edit_area(editor_state_t* s)
{
    Rectangle edit_area_rect = edit_area_get_rect(&(s->edit_area), s->window_w, s->window_h);

    // account for border thickness since it takes from the original rect
    // without this an edit area of border thickness 12 is smaller than of 6
    edit_area_rect.x -= s->edit_area.border_thickness;
    edit_area_rect.y -= s->edit_area.border_thickness;
    edit_area_rect.width += s->edit_area.border_thickness * 2; // i don't know why this works
    edit_area_rect.height += s->edit_area.border_thickness * 2;

    DrawRectangleLinesEx(edit_area_rect, (float)(s->edit_area.border_thickness), BLACK);

    DrawRectangle(0, 0, (int)(edit_area_rect.x), s->window_h, s->bg_color);
    DrawRectangle(0, 0, s->window_w, (int)(edit_area_rect.y), s->bg_color);

    DrawRectangle((int)(edit_area_rect.x + edit_area_rect.width), 0,
                (int)(s->window_w - edit_area_rect.x), s->window_h,
                s->bg_color);

    DrawRectangle(0, (int)(edit_area_rect.y + edit_area_rect.height),
                s->window_w, (int)(s->window_h - edit_area_rect.height),
                s->bg_color);
}

static void editor_draw_tilemap(editor_state_t* s)
{
    for (int tile_y = 0; tile_y < s->tilemap.height; tile_y++)
    {
        for (int tile_x = 0; tile_x < s->tilemap.width; tile_x++)
        {
            int x = tile_x * s->tile_w - s->camera_x;
            int y = tile_y * s->tile_h - s->camera_y;

            Rectangle tile_rect = {
                (float)x, (float)y,
                (float)s->tile_w, (float)s->tile_h
            };

            int id = tilemap_get(&(s->tilemap), tile_x, tile_y);
            if (id != TILE_EMPTY)
            {
                const tile_t* tile = tileset_get_tile(&(s->tileset), id);
                if (tile == NULL)
                {
                    continue;
                }

                Texture* texture = asset_get_texture(tile->texture_path);
                if (texture == NULL)
                {
                    continue;
                }

                Rectangle src = {
                    0, 0,
                    (float)(texture->width), (float)(texture->height)};
                Vector2 origin = {0, 0};
                DrawTexturePro(*texture, src, tile_rect, origin, 0, WHITE);
            }

            Color outline_color = {
                255, 255, 255, 20
            };
            DrawRectangleLinesEx(tile_rect, 1, outline_color);
        }
    }
}