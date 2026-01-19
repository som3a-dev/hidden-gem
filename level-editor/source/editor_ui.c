#include "nk_raylib.h"
#include "editor_ui.h"
#include "log.h"

#include <assert.h>

typedef struct
{
    int raylib_key;
    int nk_key;
    bool modifier;
} nk_raylib_key_check_t;

struct nk_raylib_input_keyboard_check {
    int key;
    int input_key;
    bool modifier;
};

static void editor_update_ui_input(editor_state_t* s)
{
    struct nk_context* ctx = &(s->nk_ctx);
    ctx->delta_time_seconds = GetFrameTime();

    nk_input_begin(ctx);
    // Mouse
    const int mouseX = (int)((float)GetMouseX());
    const int mouseY = (int)((float)GetMouseY());

    if ((mouseX != s->prev_mouse_pos.x) || (mouseY != s->prev_mouse_pos.y))
    {
        s->mouse_moved = true;
    }
    else
    {
        s->mouse_moved = false;
    }
    s->prev_mouse_pos = (Vector2){(float)mouseX, (float)mouseY};

    nk_input_motion(ctx, mouseX, mouseY);
    nk_input_button(ctx, NK_BUTTON_LEFT, mouseX, mouseY, IsMouseButtonDown(MOUSE_LEFT_BUTTON));
    nk_input_button(ctx, NK_BUTTON_RIGHT, mouseX, mouseY, IsMouseButtonDown(MOUSE_RIGHT_BUTTON));
    nk_input_button(ctx, NK_BUTTON_MIDDLE, mouseX, mouseY, IsMouseButtonDown(MOUSE_MIDDLE_BUTTON));

    // Mouse Wheel
    float mouseWheel = GetMouseWheelMove();
    if (mouseWheel != 0.0f) {
        struct nk_vec2 mouseWheelMove;
        mouseWheelMove.x = 0.0f;
        mouseWheelMove.y = mouseWheel;
        nk_input_scroll(ctx, mouseWheelMove);
    }

    bool control = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    bool command = IsKeyDown(KEY_LEFT_SUPER);
    bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    struct nk_raylib_input_keyboard_check checks[] = {
        (struct nk_raylib_input_keyboard_check) {KEY_DELETE, NK_KEY_DEL, true},
        (struct nk_raylib_input_keyboard_check) {KEY_ENTER, NK_KEY_ENTER, true},
//        (struct nk_raylib_input_keyboard_check) {KEY_BACKSPACE, NK_KEY_BACKSPACE, true},
        (struct nk_raylib_input_keyboard_check) {KEY_C, NK_KEY_COPY, (control || command)},
        (struct nk_raylib_input_keyboard_check) {KEY_V, NK_KEY_PASTE, (control || command)},
        (struct nk_raylib_input_keyboard_check) {KEY_B, NK_KEY_TEXT_LINE_START, (control || command)},
        (struct nk_raylib_input_keyboard_check) {KEY_E, NK_KEY_TEXT_LINE_END, (control || command)},
        (struct nk_raylib_input_keyboard_check) {KEY_Z, NK_KEY_TEXT_UNDO, (control || command)},
        (struct nk_raylib_input_keyboard_check) {KEY_R, NK_KEY_TEXT_REDO, (control || command)},
        (struct nk_raylib_input_keyboard_check) {KEY_A, NK_KEY_TEXT_SELECT_ALL, (control || command)},
        (struct nk_raylib_input_keyboard_check) {KEY_LEFT, NK_KEY_TEXT_WORD_LEFT, (control || command)},
        (struct nk_raylib_input_keyboard_check) {KEY_RIGHT, NK_KEY_TEXT_WORD_RIGHT, (control || command)},
        (struct nk_raylib_input_keyboard_check) {KEY_RIGHT, NK_KEY_RIGHT, true},
        (struct nk_raylib_input_keyboard_check) {KEY_LEFT, NK_KEY_LEFT, true},
        (struct nk_raylib_input_keyboard_check) {KEY_UP, NK_KEY_UP, true},
        (struct nk_raylib_input_keyboard_check) {KEY_DOWN, NK_KEY_DOWN, true}
    };

    const int checks_count = sizeof(checks) / sizeof(struct nk_raylib_input_keyboard_check);
    bool checked = false;
    for (int i = 0; i < checks_count; i++) {
        struct nk_raylib_input_keyboard_check check = checks[i];
        if (IsKeyDown(check.key) && check.modifier) {
            nk_input_key(ctx, (enum nk_keys)check.input_key, true);
            checked = true;
        } else {
            nk_input_key(ctx, (enum nk_keys)check.input_key, false);
        }
    }
    #undef NK_RAYLIB_INPUT_KEYBOARD_CHECK_NUM

    nk_input_key(ctx, NK_KEY_SHIFT, shift);

    if (checked) {
        return;
    }

    nk_input_key(ctx, NK_KEY_LEFT, IsKeyDown(KEY_LEFT));
    nk_input_key(ctx, NK_KEY_RIGHT, IsKeyDown(KEY_RIGHT));
    nk_input_key(ctx, NK_KEY_UP, IsKeyDown(KEY_UP));
    nk_input_key(ctx, NK_KEY_DOWN, IsKeyDown(KEY_DOWN));
    nk_input_key(ctx, NK_KEY_TEXT_START, IsKeyDown(KEY_HOME));
    nk_input_key(ctx, NK_KEY_TEXT_END, IsKeyDown(KEY_END));
    nk_input_key(ctx, NK_KEY_SCROLL_START, IsKeyDown(KEY_HOME) && control);
    nk_input_key(ctx, NK_KEY_SCROLL_END, IsKeyDown(KEY_END) && control);
    nk_input_key(ctx, NK_KEY_SCROLL_DOWN, IsKeyDown(KEY_PAGE_DOWN));
    nk_input_key(ctx, NK_KEY_SCROLL_UP, IsKeyDown(KEY_PAGE_UP));

    // Functions
    if (IsKeyPressed(KEY_TAB)) nk_input_unicode(ctx, 9);

    // Unicode
    int code;
    while ((code = GetCharPressed()) != 0)
        nk_input_unicode(ctx, (nk_rune)code);


    // Edge case for backspace, because IsKeyDown() isn't quite the right behavior
    // Maybe report this to the raylib nuklear implementation on github
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE))
    {
        nk_input_key(ctx, NK_KEY_BACKSPACE, true);
    }
    else
    {
        nk_input_key(ctx, NK_KEY_BACKSPACE, false);
    }

    nk_input_end(ctx);
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
        nk_layout_row_begin(ctx, NK_STATIC, rect.h * 0.7f, 10);
        nk_layout_row_push(ctx, (rect.h * 2.0f));
        if (nk_button_label(ctx, "Open"))
        {
            editor_open_map(s, "map.lem");
        }
        if (nk_button_label(ctx, "Save"))
        {
            editor_save_map(s, "map.lem");
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
    if (nk_begin(ctx, "Options", rect, 0))
    {
        nk_style_push_font(ctx, &(s->nk_inner_font));

        nk_layout_row_dynamic(ctx, 0, 1);
        nk_button_label(ctx, "This is a button");

        static int width = 1;
        width = nk_propertyi(ctx, "Width", 1, width, 48, 1, 1);

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
    (s);
}
