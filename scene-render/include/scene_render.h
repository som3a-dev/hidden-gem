/*
    This project is the library (name: scene render) responsible for drawing/rendering game scenes for both the game and map editor
    If we need the thing to be rendered to be visually consistent between the game and map editor (i.e tiles), it belongs here

    This file (scene_render.h) is the main header for the library.

    The API is abbreviated as sr or SR.

    For now, since we only have one hardcoded main scene, values like the draw buffer size (logical resolution) and tile size are hardcoded in this file
    and the game/editor are not allowed to change them.
*/

#ifndef _SCENE_RENDER_H
#define _SCENE_RENDER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <raylib.h>

// The tile size
#define SR_TILE_W 24
#define SR_TILE_H 24

// The draw buffer size (logical resolution)
#define SR_DRAW_W 640
#define SR_DRAW_H 360

/*
* This is the draw buffer where scene elements get drawn, then the buffer gets drawn to the screen
* The dimensions of this buffer are the logical resolution of the scene
*/
typedef struct
{
    RenderTexture tex;
    int w;
    int h;
} sr_scene_draw_buffer;

sr_scene_draw_buffer sr_create_draw_buffer();
void sr_destroy_draw_buffer(sr_scene_draw_buffer* buf);

/*
* Draws the scene's draw buffer into a destination rectangle on the screen
*/
void sr_draw_scene(const sr_scene_draw_buffer* buf, Rectangle dst, Color tint);

#ifdef __cplusplus
}
#endif

#endif