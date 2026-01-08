#include "nk_raylib.h"

#include "editor_ui.h"

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

    Rectangle edit_area_r = edit_area_get_rect(&(s->edit_area), s->window_w, s->window_h);
    struct nk_rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = edit_area_r.x - s->edit_area.border_thickness - 50;
    rect.h = (float)(s->window_h);

    if (nk_begin(ctx, "Tileset", rect, NK_WINDOW_BORDER | NK_WINDOW_TITLE))
    {
        nk_style_push_font(ctx, &(s->nk_font));

        nk_layout_row_dynamic(ctx, 0, 4);

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

        nk_style_pop_font(ctx);
    }
    nk_end(ctx);
}
