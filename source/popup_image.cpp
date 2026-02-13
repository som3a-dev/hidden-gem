#include "popup_image.h"
#include "game.h"

#include <assert.h>
#include <string.h>
#include <string>

const float popup_speed = 4.0f;

static void draw_boxed_text(Game* game, const Font& font, const std::string& text, Vector2 text_pos, int textsz, Rectangle box, float spacing, Color color)
{
    int words_count;
    char** words = TextSplit(text.c_str(), ' ', &words_count);

    float char_x = text_pos.x;
    float char_y = text_pos.y;
    for (int i = 0; i < words_count; i++)
    {
        const char* word = words[i];
        Vector2 word_sz = MeasureTextEx(font, word, (float)textsz, spacing);

        if ((char_x + word_sz.x) >= (box.x + box.width))
        {
            char_x = text_pos.x;
            char_y += word_sz.y;
        }

        if ((char_y + word_sz.y) >= (box.y + box.height))
        {

        }

        for (int j = 0; j < strlen(word); j++)
        {
//            DrawTextCodepoint(font, word[j], {char_x, char_y}, (float)textsz, color);
            char chars[2] = {word[j], '\0'};
            game->QueueTextEx(font, chars, {char_x, char_y}, (float)textsz, spacing, color);

            Vector2 size = MeasureTextEx(font, chars, (float)textsz, 0);
            char_x += size.x + spacing;

            if ((char_x + size.x) >= (box.x + box.width))
            {
                char_x = text_pos.x;
                char_y += size.y;
            }
        }

        char chars[] = {' ', '\0'};
        Vector2 size = MeasureTextEx(font, chars, (float)textsz, 0);
        char_x += size.x;
    }
}

void PopupImage::update(float dt)
{
    if (!visible)
    {
        return;
    }

    switch (state)
    {
        case PopupImageState::Popup:
        {
            anim_scale += popup_speed * dt;
            if (anim_scale > 1)
            {
                anim_scale = 1;
                state = PopupImageState::Shown;
            }
        } break;

        case PopupImageState::Popdown:
        {
            anim_scale -= popup_speed * dt;
            if (anim_scale < 0)
            {
                anim_scale = 0;
                state = PopupImageState::Hidden;
            }
        } break;

        default:
        {

        } break;
    }

}

void PopupImage::draw(Game* game, const Font& font, const GameDrawBuffer& draw_buf) const
{
    assert(game);
    assert(tex);

    if (!visible)
    {
        return;
    }

    if (state == PopupImageState::Hidden)
    {
        return;
    }

    float rl_scale = scale * anim_scale;

    Vector2 pos;
    pos.x = (float)(draw_buf.w / 2) - (tex->width * rl_scale / 2);
    pos.y = (float)(draw_buf.h / 2) - (tex->height * rl_scale / 2);

    Color color;
    color.r = 0x12;
    color.g = 0x11;
    color.b = 0x0f;
    color.a = 0xff;

    DrawTextureEx(*tex, pos, 0, rl_scale, WHITE);

    if (state == PopupImageState::Shown)
    {
        const float spacing = 1;

        Vector2 text_pos = {pos.x + 4, pos.y + 4};
        Vector2 text_shadow_pos = {text_pos.x + 1, text_pos.y + 1};
        const int text_sz = 20;

//        DrawTextEx(font, "Mission 1: ", text_shadow_pos, 20, spacing, BLACK);
        game->QueueTextEx(font, "Mission 1: ", text_pos, 20, spacing, color);

        Vector2 body_pos = {text_pos.x, text_pos.y + 36};
        Vector2 body_shadow_pos = {body_pos.x + 1, body_pos.y + 1};
        const std::string body = "The next scroll is hidden inside the Pharaoh`s chariot. Can you uncover How many spokes does each wheel have, and how does this design help?";

/*        draw_boxed_text(font, body, body_shadow_pos,
        10,
        {
            body_shadow_pos.x, body_shadow_pos.y,
            (float)(tex->width * scale), (float)(tex->height * scale)
        },
        spacing, BLACK);*/
        draw_boxed_text(game, font, body, body_pos,
        16,
        {
            body_pos.x, body_pos.y,
            (float)(tex->width * scale), (float)(tex->height * scale)
        },
        spacing, color);
    }
}

void PopupImage::flip()
{
    switch (state)
    {
        case PopupImageState::Shown:
        {
            state = PopupImageState::Popdown;
        } break;

        case PopupImageState::Hidden:
        {
            state = PopupImageState::Popup;
        } break;

        default:
        {

        } break;
    }
}
