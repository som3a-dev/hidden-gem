#include "nk_raylib.h"
#include "log.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

// Spacing is determined by the font size multiplied by RAYLIB_NUKLEAR_FONT_SPACING_RATIO.
#define RAYLIB_NUKLEAR_FONT_SPACING_RATIO 0.01f

static Color nk_to_rl_color(struct nk_color color);
static float nk_font_width_callback(nk_handle handle, float height, const char *text, int len);

struct nk_user_font nk_raylib_create_user_font(const Font* font, int height)
{
	struct nk_user_font nk_font = {0};

    if (font == NULL) return nk_font;

	nk_font.height = (float)(height);	
	nk_font.userdata.ptr = (void*)(font);
	nk_font.width = nk_font_width_callback;

    return nk_font;
}

struct nk_color rl_to_nk_color(Color color)
{
    struct nk_color clr;
    clr.r = color.r;
    clr.g = color.g;
    clr.b = color.b;
    clr.a = color.a;
    return clr;
}

struct nk_image nk_raylib_texture_to_image(Texture* tex)
{
    struct nk_image img = {0};
    if (tex == NULL)
    {
        return img;
    }

	img.handle.ptr = tex;
	img.w = (nk_ushort)tex->width;
	img.h = (nk_ushort)tex->height;

    // Set the region so we can sub-select the image later.
    img.region[0] = (nk_ushort)0;
    img.region[1] = (nk_ushort)0;
    img.region[2] = img.w;
    img.region[3] = img.h;

    return img;
}

void nk_raylib_free_image(struct nk_image* img)
{
    assert(img);

    free(img->handle.ptr);
}

void nk_raylib_draw_commands(struct nk_context* ctx)
{
    if (ctx == NULL) return;

    const struct nk_command* cmd = NULL;

//    #define NK_RAYLIB_DEBUG_DRAW
    nk_foreach(cmd, ctx)
    {
        switch (cmd->type)
        {
            case NK_COMMAND_LINE: 
            {
                const struct nk_command_line* line = (const struct nk_command_line*)cmd;
                DrawLine(line->begin.x, line->begin.y, line->end.x, line->end.y, nk_to_rl_color(line->color));

                #ifdef NK_RAYLIB_DEBUG_DRAW
                LOG_INFO("NK_COMMAND_LINE");
                #endif
            } break;

            case NK_COMMAND_RECT:
            {
                const struct nk_command_rect* rect = (const struct nk_command_rect*)cmd;
                Rectangle rl_rect = {
                    rect->x, rect->y,
                    rect->w, rect->h
                };
                DrawRectangleLinesEx(rl_rect, rect->line_thickness, nk_to_rl_color(rect->color));

                #ifdef NK_RAYLIB_DEBUG_DRAW
                LOG_INFO("NK_COMMAND_RECT");
                #endif
            } break;

            case NK_COMMAND_RECT_FILLED:
            {
                const struct nk_command_rect_filled* r = (const struct nk_command_rect_filled*)cmd;
                Color color = nk_to_rl_color(r->color);
                Rectangle rect = CLITERAL(Rectangle) {(float)r->x, (float)r->y, (float)r->w, (float)r->h};
                float roundness = (rect.width > rect.height) ?
                        ((2 * r->rounding)/rect.height) : ((2 * r->rounding)/rect.width);
                roundness = NK_CLAMP(0.0f, roundness, 1.0f);
                if (roundness > 0.0f) {
                    DrawRectangleRounded(rect, roundness, 20, color);
                }
                else {
                    DrawRectangleRec(rect, color);
                }

                #ifdef NK_RAYLIB_DEBUG_DRAW
                LOG_INFO("NK_COMMAND_RECT_FILLED");
                #endif
            } break;

            case NK_COMMAND_RECT_MULTI_COLOR:
            {
                const struct nk_command_rect_multi_color* c = (const struct nk_command_rect_multi_color*)cmd;
                Color bottom = nk_to_rl_color(c->bottom);
                Color top = nk_to_rl_color(c->top);
                Color left = nk_to_rl_color(c->left);
                Color right = nk_to_rl_color(c->right);
                Rectangle rect = {(float)(c->x), (float)(c->y), (float)(c->w), (float)(c->h)};

                DrawRectangleGradientEx(rect, left, bottom, right, top);
                #ifdef NK_RAYLIB_DEBUG_DRAW
                LOG_INFO("NK_COMMAND_RECT_MULTI_COLOR");
                #endif
            } break;

            case NK_COMMAND_CIRCLE_FILLED:
            {
                const struct nk_command_circle_filled* circle = (const struct nk_command_circle_filled*)cmd;
                DrawEllipse(circle->x + (circle->w / 2), circle->y + (circle->h / 2),
                            (float)(circle->w / 2), (float)(circle->h / 2),
                            nk_to_rl_color(circle->color));

                #ifdef NK_RAYLIB_DEBUG_DRAW
                LOG_INFO("NK_COMMAND_CIRCLE_FILLED");
                #endif
            } break;

            case NK_COMMAND_TRIANGLE_FILLED:
            {
                const struct nk_command_triangle_filled* c = (const struct nk_command_triangle_filled*)cmd;

                Vector2 p1 = {(float)(c->b.x), (float)(c->b.y)};
                Vector2 p2 = {(float)(c->a.x), (float)(c->a.y)};
                Vector2 p3 = {(float)(c->c.x), (float)(c->c.y)};
                Color color = nk_to_rl_color(c->color);

                DrawTriangle(p1, p2, p3, color);

                #ifdef NK_RAYLIB_DEBUG_DRAW
                LOG_INFO("NK_COMMAND_TRIANGLE_FILLED");
                #endif
            } break;

            case NK_COMMAND_TEXT:
            {

                const struct nk_command_text *text = (const struct nk_command_text*)cmd;
                Color color = nk_to_rl_color(text->foreground);
                float fontSize = text->font->height;
                Font* font = (Font*)text->font->userdata.ptr;
                if (font != NULL) {
                    Vector2 position = {(float)text->x, (float)text->y};
                    DrawTextEx(*font, (const char*)text->string, position, fontSize, fontSize * RAYLIB_NUKLEAR_FONT_SPACING_RATIO, color);
                }
                else {
                    DrawText((const char*)text->string, (int)(text->x), (int)(text->y), (int)fontSize, color);
                }

                #ifdef NK_RAYLIB_DEBUG_DRAW
                LOG_INFO("NK_COMMAND_TEXT");
                #endif
            } break;

            case NK_COMMAND_SCISSOR:
            {
                const struct nk_command_scissor* scissor = (const struct nk_command_scissor*)cmd;

                EndScissorMode();
                BeginScissorMode(scissor->x, scissor->y, scissor->w, scissor->h);

                #ifdef NK_RAYLIB_DEBUG_DRAW
                LOG_INFO("NK_COMMAND_SCISSOR");
                #endif
            } break;

            case NK_COMMAND_IMAGE:
            {
                const struct nk_command_image* c = (const struct nk_command_image*)cmd;

                Texture* tex = (Texture*)(c->img.handle.ptr);
                if (tex)
                {
                    Rectangle src = {0, 0, (float)(tex->width), (float)(tex->height)};
                    Rectangle dst = {(float)(c->x), (float)(c->y), (float)(c->w), (float)(c->h)};

                    Color color;
                    color.r = c->col.r;
                    color.g = c->col.g;
                    color.b = c->col.b;
                    color.a = c->col.a;
                    DrawTexturePro(*tex, src, dst, (Vector2){0,0}, 0, color);
                }

                #ifdef NK_RAYLIB_DEBUG_DRAW
                LOG_INFO("NK_COMMAND_IMAGE");
                #endif
            } break;

            default:
            {
                #ifdef NK_RAYLIB_DEBUG_DRAW
                LOG_INFO("UNKOWN %d", cmd->type);
                #endif
            } break;
        }
    }
    EndScissorMode();

#ifdef NK_RAYLIB_DEBUG_DRAW
    printf("\n");
#endif
}

static Color nk_to_rl_color(struct nk_color color)
{
	Color rl_color = {color.r, color.g, color.b, color.a};
	return rl_color;
}

static float nk_font_width_callback(nk_handle handle, float height, const char *text, int len)
{
    (void)len;
    (void)height;
    Font* font = (Font*)(handle.ptr);
	if (font == NULL) {
		return 0;
	}
    if (len <= 0) {
        return 0;
    }

    // Grab the text with the cropped length so that it only measures the desired string length.
    const char* subtext = TextSubtext(text, 0, len);

    // Spacing is determined by the font size multiplied by RAYLIB_NUKLEAR_FONT_SPACING_RATIO.
    // Raylib only counts the spacing between characters, but Nuklear expects one spacing to be
    // counter for every character in the string:
    float text_width = MeasureTextEx(*font, subtext, (float)(height), 0).x + (height * RAYLIB_NUKLEAR_FONT_SPACING_RATIO);

    return text_width;
}