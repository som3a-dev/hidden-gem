#include "asset.h"
#include "collision.h"
#include "editor.h"
#include "tilemap.h"
#include "nk_raylib.h"

#include "editor_cursor.h"
#include "editor_camera.h"
#include "editor_ui.h"

#include "map_format.h"
#include "scene_render.h"

#include "log.h"

#include "vendor/cJSON.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static void editor_init(editor_state_t* s);
static void editor_delete(editor_state_t* s);

static void editor_update(editor_state_t* s);

static void editor_draw(editor_state_t* s);
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
    s->window_w = 640 * 2;
    s->window_h = 360 * 2;

	InitWindow(s->window_w, s->window_h, "level-editor");
    SetTargetFPS(60);

    s->scene = sr_create_draw_buffer();

    s->tile_w = SR_TILE_W;
    s->tile_h = SR_TILE_H;

	s->bg_color.r = 20;
    s->bg_color.g = 20;
    s->bg_color.b = 20;
    s->bg_color.a = 255;

	s->font = LoadFontEx(ASSETS_PATH"DroidSans.ttf", 48, NULL, 0);
    s->nk_font = nk_raylib_create_user_font(&(s->font), 20);

//    editor_load_tileset(s, ASSETS_PATH"tileset.json");

    s->selected_tile_id = 1;

    tilemap_create(&(s->tilemap), 64, 64);

    ctx = &(s->nk_ctx);
	nk_init_default(ctx, &(s->nk_font));

    editor_open_map(s, "C:/Users/admin/source/repos/hidden-gem/assets/map.hgm");
}

static void editor_delete(editor_state_t* s)
{
    asset_delete_assets();

    tilemap_delete(&(s->tilemap));
    sr_destroy_draw_buffer(&(s->scene));

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

//    editor_update_camera(s);
//    editor_update_cursor(s);
}

static void editor_draw(editor_state_t* s)
{
    BeginDrawing();
    ClearBackground(s->bg_color);

    BeginTextureMode(s->scene.tex);

    editor_draw_tilemap(s);
//    editor_draw_edit_area(s);
    if (s->draw_cursor)
    {
//        editor_draw_cursor(s);
    }

    EndTextureMode();

    Rectangle dst = {
        0, 0,
        (float)s->window_w, (float)s->window_h
    };

    sr_draw_scene(&(s->scene), dst, WHITE);

    nk_raylib_draw_commands(ctx);

    EndDrawing();

    nk_clear(ctx);
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

                Rectangle src;
                if (tile->sheet_w == 0)
                {
                    // Not part of a spritesheet, draw the whole texture
                    src = (Rectangle){
                    0, 0,
                    (float)(texture->width), (float)(texture->height)};
                }
                else
                {
                    int sprite_w = texture->width / tile->sheet_w;
                    int sprite_h = texture->height / tile->sheet_h;
                    src.x = (float)(tile->sheet_x * sprite_w);
                    src.y = (float)(tile->sheet_y * sprite_h);
                    src.width = (float)sprite_w;
                    src.height = (float)sprite_h;
                }

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

static char* get_dir_path(const char* filepath)
{
    int last_slash_index = -1;

    for (int i = 0; i < strlen(filepath); i++)
    {
        if ((filepath[i] == '\\') || (filepath[i] == '/'))
        {
            last_slash_index = i;
        }
    }
    
    assert(last_slash_index != -1);

    last_slash_index++; //include the last slash

    char* dirpath = malloc(sizeof(char) * (last_slash_index + 1));
    memcpy(dirpath, filepath, sizeof(char) * last_slash_index);
    dirpath[last_slash_index] = '\0';

    return dirpath;
}

void editor_load_tileset(editor_state_t* s, const char* filepath)
{
    (s);
    mf_tile_t* tiles;
    int tile_count;
    mf_load_tileset(filepath, &tiles, &tile_count);

    char* filedir = get_dir_path(filepath); // the directory the tileset json file is in, is used as the asset directory

    for (int i = 0; i < tile_count; i++)
    {
        mf_tile_t* mf_tile = tiles + i;
        (mf_tile);

        size_t pathsz = sizeof(char) * (strlen(filedir) + strlen(mf_tile->texture_id) + 1);
        assert(pathsz < ASSET_MAX_PATH);

        char path[ASSET_MAX_PATH];
        strcpy(path, filedir);
        strcpy(path + strlen(filedir), mf_tile->texture_id);
        path[pathsz-1] = '\0';

        asset_load_texture(path);
        tileset_add_tile(&(s->tileset), mf_tile->id, path, mf_tile->sheet_x, mf_tile->sheet_y, mf_tile->sheet_w, mf_tile->sheet_h);
    }

    free(filedir);
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