#include "asset.h"
#include "editor.h"
#include "tilemap.h"
#include "nk_raylib.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define CURSOR_HELD_DELAY 300
#define CURSOR_HELD_MOVE_TIME 30
#define CAMERA_SPEED 4

static void editor_init(editor_state_t* s);
static void editor_delete(editor_state_t* s);

static void editor_update(editor_state_t* s);
static void editor_update_cursor(editor_state_t* s);
static int editor_move_cursor_held(editor_state_t* s, int cursor_coord, int move);

static void editor_draw(editor_state_t* s);
static void editor_draw_edit_area(editor_state_t* s);
static void editor_draw_tilemap(editor_state_t* s);
static void editor_draw_cursor(editor_state_t* s);

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

Rectangle edit_area_get_rect(const edit_area_t* a, int window_w, int window_h)
{
    if (a == NULL)
    {
        return (Rectangle){0};
    }

    return (Rectangle){
        (float)(0 + a->left),
        (float)(0 + a->top),
        (float)(window_w - a->right - a->left),
        (float)(window_h - a->bottom - a->top)
    };
}

static void editor_init(editor_state_t* s)
{
    s->window_w = 1280;
    s->window_h = 720;

	InitWindow(s->window_w, s->window_h, "level-editor");
    SetTargetFPS(60);

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

	s->font = LoadFontEx(ASSETS_PATH"ProggyClean.ttf", 24, NULL, 0);
	s->nk_font = nk_raylib_create_user_font(&(s->font));

    tilemap_create(&(s->tilemap), s->window_w / s->tile_w, s->window_h / s->tile_h);

    Rectangle edit_rect = edit_area_get_rect(&(s->edit_area), s->window_w, s->window_h);
    s->cursor_x = (int)(edit_rect.x / s->tile_w) + 1;
    s->cursor_y = (int)(edit_rect.y / s->tile_h) + 1;

    ctx = &(s->nk_ctx);
	nk_init_default(ctx, &(s->nk_font));

    asset_load_texture(ASSETS_PATH"block.png");
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
    nk_input_begin(ctx);

    Vector2 mouse_pos = GetMousePosition();
    if ((mouse_pos.x != s->prev_mouse_pos.x) || (mouse_pos.y != s->prev_mouse_pos.y))
    {
        s->mouse_moved = true;
    }
    else
    {
        s->mouse_moved = false;
    }
    s->prev_mouse_pos = mouse_pos;

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

    nk_input_end(ctx);

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

    if (s->mouse_moved)
    {
        Rectangle edit_rect = edit_area_get_rect(&(s->edit_area), s->window_w, s->window_h);
        if ((mouse_pos.x > edit_rect.x) && (mouse_pos.x < edit_rect.x + edit_rect.width))
        {
            if ((mouse_pos.y > edit_rect.y) && (mouse_pos.y < edit_rect.y + edit_rect.height))
            {
                s->cursor_x = (int)((mouse_pos.x + s->camera_x) / s->tile_w);
                s->cursor_y = (int)((mouse_pos.y + s->camera_y) / s->tile_h);
            }
        }
    }

    if (IsKeyDown(KEY_RIGHT))
    {
        s->camera_x += CAMERA_SPEED;
    }
    else if (IsKeyDown(KEY_LEFT))
    {
        s->camera_x -= CAMERA_SPEED;
    }
    if (IsKeyDown(KEY_DOWN))
    {
        s->camera_y += CAMERA_SPEED;
    }
    else if (IsKeyDown(KEY_UP))
    {
        s->camera_y -= CAMERA_SPEED;
    }

    editor_update_cursor(s);

    int id = tilemap_get(&s->tilemap, s->cursor_x, s->cursor_y);
    if (IsKeyPressed(KEY_ENTER))
    {
        if (id == TILE_EMPTY)
        {
            tilemap_set(&s->tilemap, s->cursor_x, s->cursor_y, 1);
        }
        else if (id != TILE_INVALID)
        {
            tilemap_set(&s->tilemap, s->cursor_x, s->cursor_y, TILE_EMPTY);
        }

        // Halt the cursor movement if we set a tile
        s->cursor_first_move_ms = GetTime() * 1000;
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (id == TILE_EMPTY)
        {
            tilemap_set(&s->tilemap, s->cursor_x, s->cursor_y, 1);
        }
    }
    else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        if (id != TILE_INVALID)
        {
            tilemap_set(&s->tilemap, s->cursor_x, s->cursor_y, TILE_EMPTY);
        }
    }
}

static void editor_update_cursor(editor_state_t* s)
{
    Rectangle edit_rect = edit_area_get_rect(&(s->edit_area), s->window_w, s->window_h);
    (void)edit_rect;
    if (IsKeyPressed(KEY_D))
    {
        s->cursor_x += 1;
        s->cursor_first_move_ms = GetTime() * 1000;
    }
    else if (IsKeyPressed(KEY_A))
    {
        s->cursor_x -= 1;
        s->cursor_first_move_ms = GetTime() * 1000;

/*        if (s->cursor_x >= 0)
        {
            int x = s->cursor_x * s->tile_w;
            if (x - s->camera_x < edit_rect.x)
            {
                s->camera_x -= s->tile_w;
            }
        }*/
    }

    if (IsKeyPressed(KEY_W))
    {
        s->cursor_y -= 1;
        s->cursor_first_move_ms = GetTime() * 1000;
    }
    else if (IsKeyPressed(KEY_S))
    {
        s->cursor_y += 1;
        s->cursor_first_move_ms = GetTime() * 1000;
    }

    if (IsKeyDown(KEY_D))
    {
        s->cursor_x = editor_move_cursor_held(s, s->cursor_x, 1);

    }
    else if (IsKeyDown(KEY_A))
    {
        s->cursor_x = editor_move_cursor_held(s, s->cursor_x, -1);
    }

    if (IsKeyDown(KEY_S))
    {
        s->cursor_y = editor_move_cursor_held(s, s->cursor_y, 1);
    }
    else if (IsKeyDown(KEY_W))
    {
        s->cursor_y = editor_move_cursor_held(s, s->cursor_y, -1);
    }

    if (s->cursor_x < 0) s->cursor_x = 0;
    if (s->cursor_y < 0) s->cursor_y = 0;
    if (s->cursor_x >= s->tilemap.width) s->cursor_x = s->tilemap.width-1;
    if (s->cursor_y >= s->tilemap.height) s->cursor_y = s->tilemap.height-1;
}

static int editor_move_cursor_held(editor_state_t* s, int cursor_coord, int move)
{
    double time_ms = GetTime() * 1000;
    if ((time_ms - s->cursor_first_move_ms) > CURSOR_HELD_DELAY)
    {
        if ((time_ms - s->cursor_last_move_ms) > CURSOR_HELD_MOVE_TIME)
        {
            s->cursor_last_move_ms = time_ms;

            return cursor_coord += move;
        }
    }

    return cursor_coord;
}

static void editor_draw(editor_state_t* s)
{
    BeginDrawing();
    ClearBackground(s->bg_color);

    editor_draw_tilemap(s);
    editor_draw_edit_area(s);
    editor_draw_cursor(s);

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

            if (tilemap_get(&(s->tilemap), tile_x, tile_y) != TILE_EMPTY)
            {
                Texture* texture = asset_get_texture(ASSETS_PATH"block.png");

                assert(texture);
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

static void editor_draw_cursor(editor_state_t* s)
{
    Rectangle cursor_rect = {
        (float)(s->cursor_x * s->tile_w) - s->camera_x, (float)(s->cursor_y * s->tile_h) - s->camera_y,
        (float)s->tile_w, (float)s->tile_h
    };

    Rectangle edit_rect = edit_area_get_rect(&(s->edit_area), s->window_w, s->window_h);

    if (cursor_rect.x < edit_rect.x) return;
    if (cursor_rect.y < edit_rect.y) return;
    if ((cursor_rect.x + cursor_rect.width) > (edit_rect.x + edit_rect.width)) return;
    if ((cursor_rect.y + cursor_rect.height) > (edit_rect.y + edit_rect.height)) return;

    DrawRectangleLinesEx(cursor_rect, 1, GRAY);
}