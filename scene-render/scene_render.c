#include "scene_render.h"
#include "log.h"

#include <stdio.h>
#include <raylib.h>

sr_scene_draw_buffer sr_create_draw_buffer()
{
    sr_scene_draw_buffer buf = {0};
    buf.w = SR_DRAW_W;
    buf.h = SR_DRAW_H;
    buf.tex = LoadRenderTexture(buf.w, buf.h);

    return buf;
}

void sr_destroy_draw_buffer(sr_scene_draw_buffer* buf)
{
    if (!buf) {
        LOG_ERROR("scene-render: Invalid scene draw buffer");
    }

    UnloadRenderTexture(buf->tex);
}

void sr_draw_scene(const sr_scene_draw_buffer* buf, Rectangle dst, Color tint)
{
    if (!buf) {
        LOG_ERROR("scene-render: Invalid scene draw buffer");
    }

    if ((buf->w == 0) || (buf->h == 0)) {
        LOG_ERROR("scene-render: Invalid scene draw buffer");
    }

    if (!IsRenderTextureValid(buf->tex)) {
        LOG_ERROR("scene-render: Invalid scene draw buffer");
    }

    Rectangle src = {
        0, 0,
        (float)buf->w, (float)(-buf->h)
    };

    DrawTexturePro(buf->tex.texture, src, dst, (Vector2){0, 0}, 0, tint);
}