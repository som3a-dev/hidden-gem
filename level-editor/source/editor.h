#ifndef _EDITOR_H
#define _EDITOR_H

#include "asset.h"
#include "edit_area.h"
#include "input_action.h"
#include "tilemap.h"
#include "tileset.h"
#include "nk_raylib.h"

#include <raylib.h>

#include <stdbool.h>

typedef struct
{
    int window_w;
    int window_h;

    struct nk_user_font nk_font;
    struct nk_context nk_ctx;
    Font font;

    Color bg_color;

    bool mouse_moved; // did the mouse move last frame
    Vector2 prev_mouse_pos;

    edit_area_t edit_area;

    tileset_t tileset;

    tilemap_t tilemap;
    int selected_tile_id;
    int tile_w;
    int tile_h;

    int camera_x;
    int camera_y;

    int cursor_x;
    int cursor_y;
    double cursor_first_move_ms;
    double cursor_last_move_ms;
    bool draw_cursor;
} editor_state_t;

void run_editor();

#endif
