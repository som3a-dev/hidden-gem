#ifndef _NK_RAYLIB
#define _NK_RAYLIB

#define NK_INCLUDE_DEFAULT_ALLOCATOR
#include "vendor/nuklear.h"

#include <raylib.h>

void nk_raylib_draw_commands(struct nk_context* ctx);

// Creates/Fills out a nk_user_font from a raylib Font
struct nk_user_font nk_raylib_create_user_font(const Font* font, int height);

struct nk_image nk_raylib_texture_to_image(Texture* tex);

// frees memory allocated in a nk_image returned by nk_raylib_texture_to_image
void nk_raylib_free_image(struct nk_image* img);

#endif