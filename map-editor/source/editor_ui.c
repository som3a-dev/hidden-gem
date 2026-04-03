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

void editor_update_ui(editor_state_t* s)
{
    editor_update_ui_input(s);
}

void editor_draw_ui(editor_state_t* s)
{
    struct nk_context* ctx = &(s->nk_ctx);
    assert(ctx);
    nk_raylib_draw_commands(ctx);
}