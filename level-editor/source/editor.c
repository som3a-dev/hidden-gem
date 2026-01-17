#include "asset.h"
#include "collision.h"
#include "editor.h"
#include "tilemap.h"
#include "nk_raylib.h"

#include "editor_cursor.h"
#include "editor_camera.h"
#include "editor_ui.h"

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
static void editor_update_zoom(editor_state_t* s);

static void editor_draw(editor_state_t* s);
static void editor_draw_edit_area(editor_state_t* s);
static void editor_draw_tilemap(editor_state_t* s);

static void editor_load_tileset(editor_state_t* s, const char* filepath);

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

    const int left_panel_width = (int)(s->window_w * 0.2f);
    const int top_panel_height = (int)(s->window_h * 0.07f);

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

	s->bg_color.r = 20;
    s->bg_color.g = 20;
    s->bg_color.b = 20;
    s->bg_color.a = 255;

	s->font = LoadFontEx(ASSETS_PATH"DroidSans.ttf", 24, NULL, 0);
	s->nk_font = nk_raylib_create_user_font(&(s->font), 24);
    s->nk_menu_font = nk_raylib_create_user_font(&(s->font), 16);

    editor_load_tileset(s, ASSETS_PATH"tileset.json");

    s->selected_tile_id = 1;

    tilemap_create(&(s->tilemap), s->window_w / s->tile_w, s->window_h / s->tile_h);

    Rectangle edit_rect = panel_layout_get_rect(&(s->edit_area), s->window_w, s->window_h, 4);
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
    (s);
    if (IsKeyPressed(KEY_EQUAL))
    {
    }
    else if (IsKeyPressed(KEY_MINUS))
    {
    }

/*    if (s->edit_area.border_thickness < 1)
    {
        s->edit_area.border_thickness = 1;
    }*/
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
    editor_ui_menu(s);

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

static void editor_load_tileset(editor_state_t* s, const char* filepath)
{
    (s);
    FILE* fp = fopen(filepath, "r");
    if (fp == NULL)
    {
        return;
    }

    fseek(fp, 0, SEEK_END);
    long bufsz = ftell(fp) + 1;
    fseek(fp, 0, SEEK_SET);

    char* buf = malloc(sizeof(char) * bufsz);
    fread(buf, sizeof(char), bufsz, fp);
    buf[bufsz-1] = '\0';

    cJSON* json = cJSON_Parse(buf);
    if (json == NULL)
    {
        printf("WARNING: Invalid tileset json format\n");
        return;
    }

    if (cJSON_IsArray(json) == false)
    {
        printf("WARNING: Invalid tileset json format\n");
        goto end;
    }

    for (int i = 0; i < cJSON_GetArraySize(json); i++)
    {
        cJSON* item = cJSON_GetArrayItem(json, i);
        if (cJSON_IsObject(item) == false)
        {
            printf("WARNING: Invalid tileset json format\n");
            goto end;
        }

        cJSON* texture_id_item = cJSON_GetObjectItem(item, "texture_id");
        if (cJSON_IsString(texture_id_item) == false)
        {
            printf("WARNING: Invalid tileset json format\n");
            goto end;
        }

        // TODO(): find a way to check if id is not an integer
        int id = i + 1;
        char* texture_id = cJSON_GetStringValue(texture_id_item);

        size_t pathsz = sizeof(char) * (strlen(ASSETS_PATH) + strlen(texture_id) + 1);
        char* path = malloc(pathsz);
        strcpy(path, ASSETS_PATH);
        strcpy(path + strlen(ASSETS_PATH), texture_id);
        path[pathsz-1] = '\0';

        tileset_add_tile(&(s->tileset), id, path);

        free(path);
    }

    end:
    fclose(fp);
    free(buf);
    cJSON_Delete(json);
}