#ifndef _EDITOR_H
#define _EDITOR_H

#include "tilemap.h"
#include "nk_raylib.h"

#include <raylib.h>

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

    edit_area_t edit_area;

    tilemap_t tilemap;
    int tile_w;
    int tile_h;

    int cursor_x;
    int cursor_y;
} editor_state_t;

void run_editor();

#endif
