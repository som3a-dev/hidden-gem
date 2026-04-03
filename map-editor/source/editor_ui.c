#include "nk_raylib.h"
#include "editor_ui.h"
#include "log.h"
#include "dialog.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static inline float nk_button_label_width(struct nk_context* ctx, struct nk_user_font* font, const char* text, float scale)
{
    if (!text)
    {
        return 0;
    }

    float text_width = font->width(font->userdata, font->height, text, (int)strlen(text));
    float width = text_width + (2 * (ctx->style.button.padding.x + ctx->style.button.border + ctx->style.button.rounding));

    return width * scale;
}

static inline bool nk_button_label_auto(struct nk_context* ctx, struct nk_user_font* font, const char* text)
{
    nk_layout_row_push(ctx, nk_button_label_width(ctx, font, text, 1.2f));
    return nk_button_label(ctx, text);
}

static void editor_update_ui_input(editor_state_t* s)
{
    struct nk_context* ctx = &(s->nk_ctx);

    nk_raylib_update_input(ctx);
}

void editor_update_ui(editor_state_t* s)
{
    struct nk_context* ctx = &(s->nk_ctx);
    struct nk_user_font* font = &(s->nk_font);
    editor_update_ui_input(s);

    if (nk_begin(ctx, "Menu", nk_rect(0, 0, (float)s->window_w, 64), NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_begin(ctx, NK_STATIC, s->nk_font.height, 2);

        if (nk_button_label_auto(ctx, font, "Open"))
        {
            char* file = dialog_select_file(DIALOG_SELECT_FILE_SAVE);
            editor_open_map(s, file);
            free(file);
        }
        if (nk_button_label_auto(ctx, font, "Save"))
        {
            char* file = dialog_select_file(DIALOG_SELECT_FILE_SAVE);
            editor_save_map(s, file);
            free(file);
        }

        nk_layout_row_end(ctx);
    }

    nk_end(ctx);
}

void editor_draw_ui(editor_state_t* s)
{
    struct nk_context* ctx = &(s->nk_ctx);
    assert(ctx);
    nk_raylib_draw_commands(ctx);
}