#ifndef _EDITOR_H
#define _EDITOR_H

#include "asset.h"
#include "panel.h"
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

    struct nk_context nk_ctx;
    struct nk_user_font nk_font;
    struct nk_user_font nk_menu_font;
    Font font;

    Color bg_color;

    bool mouse_moved; // did the mouse move last frame
    Vector2 prev_mouse_pos;

    panel_layout_t edit_area;
    panel_layout_t tileset_panel;
    panel_layout_t menu_panel;

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
