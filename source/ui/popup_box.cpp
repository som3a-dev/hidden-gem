#include "popup_box.h"
#include "../game.h"

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

namespace UI
{
    void PopupBox::update(Game* game)
    {
        if (!visible)
        {
            return;
        }

        switch (state)
        {
            case PopupBoxState::Popup:
            {
                anim_scale += popup_speed * game->dt;
                if (anim_scale > 1)
                {
                    anim_scale = 1;
                    state = PopupBoxState::Shown;
                }
            } break;

            case PopupBoxState::Popdown:
            {
                anim_scale -= popup_speed * game->dt;
                if (anim_scale < 0)
                {
                    anim_scale = 0;
                    state = PopupBoxState::Hidden;
                }
            } break;

            default:
            {

            } break;
        }

        // Update rect
        float rl_scale = scale * anim_scale;

        if (((w == 0) || (h == 0)) && (bg_texture))
        {
            rect.width =  bg_texture->width * rl_scale;
            rect.height = bg_texture->height * rl_scale;
        }
        else
        {
            rect.width =  w * rl_scale;
            rect.height = h * rl_scale;
        }

        rect.x = (float)(center_x) - rect.width / 2;
        rect.y = (float)(center_y) - rect.height / 2;
    }

    void PopupBox::draw(Game* game, const Font& font) const
    {
        assert(game);

        if (!visible)
        {
            return;
        }

        if (state == PopupBoxState::Hidden)
        {
            return;
        }

        if (bg_texture)
        {
            DrawTexturePro(*bg_texture,
            {0, 0, (float)(bg_texture->width), (float)(bg_texture->height)},
            rect, {0,0}, 0, WHITE);
        }
        else
        {
            DrawRectanglePro(rect, {0, 0}, 0, bg_color);
        }

        if (state == PopupBoxState::Shown)
        {
            const int header_sz = 20;
            const int body_sz = 16;
            const float text_spacing = 1;
            Vector2 header_pos = {rect.x + 4, rect.y + 4};
            Vector2 body_pos = {header_pos.x, header_pos.y + header_sz * 2};

            game->QueueTextEx(font, header.c_str(), header_pos, header_sz, text_spacing, text_color);

            draw_boxed_text(game, font, body, body_pos, body_sz, {
                body_pos.x, body_pos.y,
                rect.width, rect.height
            },
            text_spacing, text_color);
        }
    }

    void PopupBox::flip()
    {
        switch (state)
        {
            case PopupBoxState::Shown:
            {
                state = PopupBoxState::Popdown;
            } break;

            case PopupBoxState::Hidden:
            {
                state = PopupBoxState::Popup;
            } break;

            default:
            {

            } break;
        }
    }
}