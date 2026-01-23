#include "nk_raylib.h"
#include "editor_ui.h"
#include "log.h"
#include "dialog.h"

#include <assert.h>
#include <stdlib.h>

static void editor_update_ui_input(editor_state_t* s)
{
    struct nk_context* ctx = &(s->nk_ctx);
    assert(ctx);
    nk_raylib_update_input(ctx);
}

static void editor_ui_tileset(editor_state_t* s)
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

    nk_style_push_font(ctx, &(s->nk_title_font));
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

        nk_layout_row_dynamic(ctx, 256, 1);
    }
    nk_style_pop_font(ctx);
    nk_end(ctx);
}

static void editor_ui_menu(editor_state_t* s)
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
//        nk_layout_row_static(ctx, rect.h * 0.7f, (int)(rect.h * 2.0f), 3);
        nk_layout_row_begin(ctx, NK_STATIC, rect.h * 0.6f, 10);
//        nk_layout_row_static(ctx, 0, 128, 2);
        nk_layout_row_push(ctx, (rect.h * 2.0f));
        if (nk_button_label(ctx, "Open Map"))
        {
            char* filepath = dialog_select_file(DIALOG_SELECT_FILE_OPEN);
            editor_open_map(s, filepath);
            free(filepath);
        }
        if (nk_button_label(ctx, "Save Map"))
        {
            char* filepath = dialog_select_file(DIALOG_SELECT_FILE_SAVE);
            editor_save_map(s, filepath);
            free(filepath);
        }

        nk_layout_row_end(ctx);

    }
    nk_style_pop_font(ctx);
    nk_end(ctx);
}

static void editor_ui_right_panel(editor_state_t* s)
{
    struct nk_context* ctx = &(s->nk_ctx);

    struct nk_rect rect;
    {
        Rectangle r = panel_layout_get_rect(&(s->right_panel), s->window_w, s->window_h, 4);
        rect = nk_rect(r.x, r.y, (float)r.width, (float)r.height);
    }

    nk_style_push_font(ctx, &(s->nk_title_font));
    if (nk_begin(ctx, "Options", rect, NK_WINDOW_TITLE | NK_WINDOW_BORDER))
    {
        nk_style_push_font(ctx, &(s->nk_inner_font));

        nk_layout_row_dynamic(ctx, 0, 1);

        static int width = 1;
        width = nk_propertyi(ctx, "Map Width: ", 1, width, 100, 1, 1);

        static int height = 1;
        height = nk_propertyi(ctx, "Map Height: ", 1, height, 50, 1, 1);

        if (nk_input_is_mouse_hovering_rect(&(ctx->input), ctx->current->bounds) == false)
        {
            width = s->tilemap.width;
            height = s->tilemap.height;
        }

        if (nk_button_label(ctx, "Apply Size"))
        {
            tilemap_resize(&(s->tilemap), width, height);
        }

        nk_style_pop_font(ctx);
    }

    nk_style_pop_font(ctx);
    nk_end(ctx);

/*    static struct nk_colorf bg = {1, 1, 1, 1};

    nk_style_push_font(ctx, &(s->nk_inner_font));
    if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
        enum {EASY, HARD};
        static int op = EASY;
        static int property = 20;
        nk_layout_row_static(ctx, 30, 80, 1);
        if (nk_button_label(ctx, "button"))
            TraceLog(LOG_INFO, "button pressed");

        nk_layout_row_dynamic(ctx, 30, 2);
        if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
        if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

        static char buffer[32] = {0};
        nk_edit_string_zero_terminated(ctx, NK_EDIT_SIMPLE, buffer, 16, nk_filter_default);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "background:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(ctx, 25, 1);

        if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx),400))) {
            nk_layout_row_dynamic(ctx, 120, 1);
            bg = nk_color_picker(ctx, bg, NK_RGBA);
            nk_layout_row_dynamic(ctx, 25, 1);
            bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
            bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
            bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
            bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
            nk_combo_end(ctx);
        }
    }
    nk_style_pop_font(ctx);

    nk_end(ctx);
    */
}

void editor_update_ui(editor_state_t* s)
{
    editor_update_ui_input(s);

    editor_ui_tileset(s);
    editor_ui_menu(s);
    editor_ui_right_panel(s);
}

void editor_draw_ui(editor_state_t* s)
{
    struct nk_context* ctx = &(s->nk_ctx);
    assert(ctx);
    nk_raylib_draw_commands(ctx);
}
