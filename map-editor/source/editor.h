#ifndef _EDITOR_H
#define _EDITOR_H

#include "asset.h"
#include "panel.h"
#include "input_action.h"
#include "tilemap.h"
#include "tileset.h"
#include "nk_raylib.h"
#include "scene_render.h"

#include <raylib.h>

#include <stdbool.h>

typedef struct
{
    int window_w;
    int window_h;

    sr_scene_draw_buffer scene;
    Rectangle scene_dst;

    struct nk_context nk_ctx;

    Font font;
    struct nk_user_font nk_font;

    Color bg_color;

    bool mouse_moved; // did the mouse move last frame
    Vector2 prev_mouse_pos;

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

// TODO(omar): move these to a editor_io component or something, they shouldn't be exposed like this
void editor_load_tileset(editor_state_t* s, const char* filepath);
void editor_open_map(editor_state_t* s, const char* filepath);
void editor_save_map(editor_state_t* s, const char* filepath);

Vector2 editor_get_logical_mouse_pos(editor_state_t* s);

#endif
