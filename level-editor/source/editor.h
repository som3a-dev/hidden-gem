#ifndef _EDITOR_H
#define _EDITOR_H

#include "tilemap.h"
#include "nk_raylib.h"

#include <raylib.h>

#include <stdbool.h>

// the edit/play area is defined as offsets from the window in the 4 directions
// left right top bottom
typedef struct
{
    int left;
    int right;
    int top;
    int bottom;
    int border_thickness;
} edit_area_t;

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

    tilemap_t tilemap;
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

// returns an {x, y, w, h} rect from the edit area, given the window/viewport width and height
Rectangle edit_area_get_rect(const edit_area_t* a, int window_w, int window_h);

#endif
