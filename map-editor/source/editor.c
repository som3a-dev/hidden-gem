#include "asset.h"
#include "collision.h"
#include "editor.h"
#include "tilemap.h"
#include "nk_raylib.h"

#include "editor_cursor.h"
#include "editor_camera.h"
#include "editor_ui.h"

#include "map_format.h"

#include "log.h"

#include "vendor/cJSON.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define TOP_PANEL_HEIGHT 48
#define LEFT_PANEL_WIDTH 256
#define UI_PADDING 12

static void editor_init(editor_state_t* s);
static void editor_delete(editor_state_t* s);

static void editor_update(editor_state_t* s);

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
    s->window_w = 1920  - (16 * 12);
    s->window_h = 1080  - ( 9 * 12);

	InitWindow(s->window_w, s->window_h, "level-editor");
    SetTargetFPS(60);

    asset_load_texture(ASSETS_PATH"block.png");
    asset_load_texture(ASSETS_PATH"dirt0.png");

    s->tile_w = 32;
    s->tile_h = 32;

    const int left_panel_width = (int)(s->window_w * 0.1f);
    const int right_panel_width = (int)(left_panel_width * 1.3f);
    const int top_panel_height = (int)(s->window_h * 0.05f);

    s->edit_area.right = (int)(s->window_w * 0.15f);
    s->edit_area.bottom = (int)(s->window_h * 0.2f);
    s->edit_area.top = (int)(s->window_h * 0.15f);
    s->edit_area.left = (int)(left_panel_width * 1.05f + UI_PADDING);

    s->tileset_panel.right = s->window_w - left_panel_width;
    s->tileset_panel.left = UI_PADDING;
    s->tileset_panel.bottom = (int)(s->window_h * 0.02f);
    s->tileset_panel.top = top_panel_height + UI_PADDING;

    s->menu_panel.left = UI_PADDING;
    s->menu_panel.right = UI_PADDING;
    s->menu_panel.top = UI_PADDING;
    s->menu_panel.bottom = s->window_h - top_panel_height;

    s->right_panel.left = (int)(s->window_w - right_panel_width);
    s->right_panel.right = UI_PADDING;
    s->right_panel.bottom = UI_PADDING;
    s->right_panel.top = top_panel_height + UI_PADDING;

	s->bg_color.r = 20;
    s->bg_color.g = 20;
    s->bg_color.b = 20;
    s->bg_color.a = 255;

	s->font = LoadFontEx(ASSETS_PATH"DroidSans.ttf", 48, NULL, 0);
	s->nk_title_font = nk_raylib_create_user_font(&(s->font), 24);
    s->nk_menu_font = nk_raylib_create_user_font(&(s->font), 20);
    s->nk_inner_font = nk_raylib_create_user_font(&(s->font), 20);

//    editor_load_tileset(s, ASSETS_PATH"tileset.json");

    s->selected_tile_id = 1;

    tilemap_create(&(s->tilemap), 48, 48);

    Rectangle edit_rect = panel_layout_get_rect(&(s->edit_area), s->window_w, s->window_h, 4);
    s->cursor_x = (int)(edit_rect.x / s->tile_w) + 1;
    s->cursor_y = (int)(edit_rect.y / s->tile_h) + 1;

    s->camera_x = -(s->edit_area.left + s->tile_w);
    s->camera_y = -(s->edit_area.top + s->tile_h);

    ctx = &(s->nk_ctx);
	nk_init_default(ctx, &(s->nk_title_font));
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
    const int mouseX = (int)((float)GetMouseX());
    const int mouseY = (int)((float)GetMouseY());

    if ((mouseX != s->prev_mouse_pos.x) || (mouseY != s->prev_mouse_pos.y))
    {
        s->mouse_moved = true;
    }
    else
    {
        s->mouse_moved = false;
    }
    s->prev_mouse_pos = (Vector2){(float)mouseX, (float)mouseY};

    editor_update_ui(s);
    editor_update_camera(s);
    editor_update_cursor(s);
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

    editor_draw_ui(s);

    nk_raylib_draw_commands(ctx);

    EndDrawing();

    nk_clear(ctx);
}

static void editor_draw_edit_area(editor_state_t* s)
{
    Rectangle edit_area_rect = panel_layout_get_rect(&(s->edit_area), s->window_w, s->window_h, 4);

    DrawRectangleLinesEx(edit_area_rect, 4, BLACK);

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

void editor_load_tileset(editor_state_t* s, const char* filepath)
{
    mf_tile_t* tiles;
    int tile_count;
    mf_load_tileset(filepath, &tiles, &tile_count);

    for (int i = 0; i < tile_count; i++)
    {
        mf_tile_t* mf_tile = tiles + i;

        size_t pathsz = sizeof(char) * (strlen(ASSETS_PATH) + strlen(mf_tile->texture_id) + 1);
        assert(pathsz < ASSET_MAX_PATH);

        char path[ASSET_MAX_PATH];
        strcpy(path, ASSETS_PATH);
        strcpy(path + strlen(ASSETS_PATH), mf_tile->texture_id);
        path[pathsz-1] = '\0';
        tileset_add_tile(&(s->tileset), mf_tile->id, path);
    }

    mf_load_tileset_free(&tiles, tile_count);
}

void editor_open_map(editor_state_t* s, const char* filepath)
{
    LOG_INFO("Loading map from '%s'", filepath);

    mf_tilemap_t map = mf_load_tilemap(filepath);
    if (map.tiles == NULL)
    {
        return;
    }

    tilemap_resize(&(s->tilemap), map.w, map.h);

    for (mf_mapsz_t y = 0; y < map.h; y++)
    {
        for (mf_mapsz_t x = 0; x < map.w; x++)
        {
            mf_tileid_t tile = mf_tilemap_get(&map, x, y);
            if (tile == MF_TILE_EMPTY)
            {
                tilemap_set(&(s->tilemap), x, y, TILE_EMPTY);
            }
            else
            {
                tilemap_set(&(s->tilemap), x, y, tile);
            }
        }
    }

    mf_tilemap_destroy(&map);

    const char* dir_path = GetDirectoryPath(filepath);
    const char* tileset_file = "\\tileset.json";

    size_t tileset_path_len = strlen(tileset_file) + strlen(dir_path) + 1;
    char* tileset_path = malloc(sizeof(char) * tileset_path_len);

    tileset_path[tileset_path_len-1] = '\0';
    memcpy(tileset_path, dir_path, strlen(dir_path));
    memcpy(tileset_path + strlen(dir_path), tileset_file, strlen(tileset_file));

    LOG_INFO("Looking for tileset.json in path '%s'", tileset_path);

    editor_load_tileset(s, tileset_path);

    free(tileset_path);
}

void editor_save_map(editor_state_t* s, const char* filepath)
{
    LOG_INFO("Saving map to '%s'", filepath);

    mf_tilemap_t map = mf_tilemap_create(s->tilemap.width, s->tilemap.height);
    for (int y = 0; y < s->tilemap.height; y++)
    {
        for (int x = 0; x < s->tilemap.width; x++)
        {
            int tile = tilemap_get(&(s->tilemap), x, y);
            if (tile == TILE_EMPTY)
            {
                mf_tilemap_set(&map, x, y, MF_TILE_EMPTY);
            }
            else
            {
                mf_tilemap_set(&map, x, y, tile);
            }
        }
    }

    mf_save_tilemap(filepath, &map);

    mf_tilemap_destroy(&map);
}