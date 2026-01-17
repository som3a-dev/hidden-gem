#include "nk_raylib.h"
#include "editor_ui.h"

#include <assert.h>

void editor_update_ui_input(editor_state_t* s)
{
    struct nk_context* ctx = &(s->nk_ctx);

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
}

void editor_ui_tileset(editor_state_t* s)
{
    struct nk_context* ctx = &(s->nk_ctx);
    assert(ctx);

    struct nk_rect rect;
    {
        Rectangle r = panel_layout_get_rect(&(s->tileset_panel), s->window_w, s->window_h, 4);
        rect = nk_rect(r.x, r.y, (float)r.width, (float)r.height);
    }

    struct nk_color color;
    color.r = 0;
    color.g = 0;
    color.b = 0;
    color.a = 255;

    nk_style_push_font(ctx, &(s->nk_font));
    if (nk_begin(ctx, "Tileset", rect, NK_WINDOW_BORDER | NK_WINDOW_TITLE))
    {
        struct nk_rect content_rect = nk_window_get_content_region(ctx);

        int tiles_in_row = (int)(content_rect.w / s->tile_w) - 1;
        nk_layout_row_static(ctx, (float)(s->tile_h), s->tile_w, tiles_in_row);

        for (int i = 0; i < TILES_CAPACITY; i++)
        {
            tile_t* tile = s->tileset.tiles + i;
            if (tile->id == SET_TILE_EMPTY)
            {
                continue;
            }

            Texture* tex = asset_get_texture(tile->texture_path);
            if (tex == NULL)
            {
                continue;
            }
            struct nk_image img = nk_raylib_texture_to_image(tex);

            struct nk_rect r;
            struct nk_color c = {255, 255, 255, 255};
            if (nk_widget_is_hovered(ctx))
            {
                c.a = 200;
            }
            if (nk_widget_is_mouse_clicked(ctx, NK_BUTTON_LEFT))
            {
                s->selected_tile_id = tile->id;
            }

            nk_widget(&r, ctx);
            nk_draw_image(nk_window_get_canvas(ctx), r, &img, c);
        }
    }
    nk_style_pop_font(ctx);
    nk_end(ctx);
}

void editor_ui_menu(editor_state_t* s)
{
    struct nk_context* ctx = &(s->nk_ctx);

    struct nk_rect rect;
    {
        Rectangle r = panel_layout_get_rect(&(s->menu_panel), s->window_w, s->window_h, 4);
        rect = nk_rect(r.x, r.y, (float)r.width, (float)r.height);
    }

    nk_style_push_font(ctx, &(s->nk_menu_font));
    if (nk_begin(ctx, "Menu", rect, NK_WINDOW_BORDER))
    {
//        nk_layout_row_static(ctx, 24, 48, 12);
        nk_button_label(ctx, "Open");
        nk_button_label(ctx, "Save");
    }
    nk_style_pop_font(ctx);
    nk_end(ctx);
}