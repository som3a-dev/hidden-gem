#include "editor_cursor.h"
#include "collision.h"

#include <assert.h>

#define CURSOR_HELD_DELAY 300
#define CURSOR_HELD_MOVE_TIME 30

static int editor_move_cursor_held(editor_state_t* s, int cursor_coord, int move);

static void editor_update_cursor_tile(editor_state_t* s);
static bool editor_update_cursor_mouse(editor_state_t* s);
static void editor_update_cursor_keyboard(editor_state_t* s);

static bool editor_update_cursor_pos(editor_state_t* s, int new_x, int new_y)
{
    assert(s);

    if (new_x < 0) return false;
    if (new_y < 0) return false;
    if (new_x >= s->tilemap.width) return false;
    if (new_y >= s->tilemap.height) return false;

    s->cursor_x = new_x;
    s->cursor_y = new_y;

    return true;
}

static bool editor_is_mouse_in_tilemap(editor_state_t* s)
{
    Vector2 mouse_pos = GetMousePosition();
    if (is_pos_in_rect(mouse_pos.x, mouse_pos.y, panel_layout_get_rect(&(s->edit_area), s->window_w, s->window_h, 4)) == false)
    {
        return false;
    }

    int new_x  = (int)((mouse_pos.x + s->camera_x) / s->tile_w);
    int new_y  = (int)((mouse_pos.y + s->camera_y) / s->tile_h);

    if (new_x < 0) return false;
    if (new_y < 0) return false;
    if (new_x >= s->tilemap.width) return false;
    if (new_y >= s->tilemap.height) return false;

    return true;
}

void editor_update_cursor(editor_state_t* s)
{
    if (editor_is_mouse_in_tilemap(s))
    {
        s->draw_cursor = true;
        if (editor_update_cursor_mouse(s) == false)
        {
            editor_update_cursor_keyboard(s);
        }

        editor_update_cursor_tile(s);
    }
    else
    {
        s->draw_cursor = false;
    }
}

static void editor_update_cursor_tile(editor_state_t* s)
{
    int id = tilemap_get(&s->tilemap, s->cursor_x, s->cursor_y);
    if (IsKeyPressed(KEY_ENTER))
    {
        if (id == TILE_EMPTY)
        {
            tilemap_set(&s->tilemap, s->cursor_x, s->cursor_y, s->selected_tile_id);
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
            tilemap_set(&s->tilemap, s->cursor_x, s->cursor_y, s->selected_tile_id);
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

static bool editor_update_cursor_mouse(editor_state_t* s)
{
    Vector2 mouse_pos = GetMousePosition();

    if (s->mouse_moved)
    {
        Rectangle edit_rect = panel_layout_get_rect(&(s->edit_area), s->window_w, s->window_h, 4);
        if ((mouse_pos.x > edit_rect.x) && (mouse_pos.x < edit_rect.x + edit_rect.width))
        {
            if ((mouse_pos.y > edit_rect.y) && (mouse_pos.y < edit_rect.y + edit_rect.height))
            {
                int new_x  = (int)((mouse_pos.x + s->camera_x) / s->tile_w);
                int new_y  = (int)((mouse_pos.y + s->camera_y) / s->tile_h);
                return editor_update_cursor_pos(s, new_x, new_y);
            }
        }
    }

    return false;
}

static void editor_update_cursor_keyboard(editor_state_t* s)
{
    input_action_t cursor_left = {0};
    input_action_add_key(&cursor_left, KEY_LEFT);

    input_action_t cursor_right = {0};
    input_action_add_key(&cursor_right, KEY_RIGHT);

    input_action_t cursor_up = {0};
    input_action_add_key(&cursor_up, KEY_UP);

    input_action_t cursor_down = {0};
    input_action_add_key(&cursor_down, KEY_DOWN);

    int new_x = s->cursor_x;
    int new_y = s->cursor_y;

    if (input_action_is_pressed(&cursor_right))
    {
        new_x += 1;
        s->cursor_first_move_ms = GetTime() * 1000;
    }
    else if (input_action_is_pressed(&cursor_left))
    {
        new_x -= 1;
        s->cursor_first_move_ms = GetTime() * 1000;
    }
    else if (input_action_is_held(&cursor_right))
    {
        new_x = editor_move_cursor_held(s, s->cursor_x, 1);
    }
    else if (input_action_is_held(&cursor_left))
    {
        new_x = editor_move_cursor_held(s, s->cursor_x, -1);
    }

    if (input_action_is_pressed(&cursor_up))
    {
        new_y -= 1;
        s->cursor_first_move_ms = GetTime() * 1000;
    }
    else if (input_action_is_pressed(&cursor_down))
    {
        new_y += 1;
        s->cursor_first_move_ms = GetTime() * 1000;
    }
    else if (input_action_is_held(&cursor_down))
    {
        new_y = editor_move_cursor_held(s, s->cursor_y, 1);
    }
    else if (input_action_is_held(&cursor_up))
    {
        new_y = editor_move_cursor_held(s, s->cursor_y, -1);
    }

    editor_update_cursor_pos(s, new_x, new_y);
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

void editor_draw_cursor(editor_state_t* s)
{
    assert(s);

    Rectangle cursor_rect = {
        (float)(s->cursor_x * s->tile_w) - s->camera_x, (float)(s->cursor_y * s->tile_h) - s->camera_y,
        (float)s->tile_w, (float)s->tile_h
    };

    Rectangle edit_rect = panel_layout_get_rect(&(s->edit_area), s->window_w, s->window_h, 4);

    if (cursor_rect.x < edit_rect.x) return;
    if (cursor_rect.y < edit_rect.y) return;
    if ((cursor_rect.x + cursor_rect.width) > (edit_rect.x + edit_rect.width)) return;
    if ((cursor_rect.y + cursor_rect.height) > (edit_rect.y + edit_rect.height)) return;

    DrawRectangleLinesEx(cursor_rect, 1, GRAY);
}