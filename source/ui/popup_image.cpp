#include "popup_image.h"
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
    void PopupImage::update(Game* game)
    {
        if (!visible)
        {
            return;
        }

        switch (state)
        {
            case PopupImageState::Popup:
            {
                anim_scale += popup_speed * game->dt;
                if (anim_scale > 1)
                {
                    anim_scale = 1;
                    state = PopupImageState::Shown;
                }
            } break;

            case PopupImageState::Popdown:
            {
                anim_scale -= popup_speed * game->dt;
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

        // Update rect
        float rl_scale = scale * anim_scale;

        if ((w == 0) || (h == 0))
        {
            rect.width =  tex->width * rl_scale;
            rect.height = tex->height * rl_scale;
        }
        else
        {
            rect.width =  w * rl_scale;
            rect.height = h * rl_scale;
        }

        rect.x = (float)(center_x) - rect.width / 2;
        rect.y = (float)(center_y) - rect.height / 2;
    }

    void PopupImage::draw(Game* game, const Font& font) const
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

        Color color;
        color.r = 0x12;
        color.g = 0x11;
        color.b = 0x0f;
        color.a = 0xff;

        DrawTexturePro(*tex,
        {0, 0, (float)(tex->width), (float)(tex->height)},
        rect, {0,0}, 0, WHITE);

        if (state == PopupImageState::Shown)
        {
            const float spacing = 1;

            Vector2 text_pos = {rect.x + 4, rect.y + 4};

            game->QueueTextEx(font, "Mission 1: ", text_pos, 20, spacing, color);

            Vector2 body_pos = {text_pos.x, text_pos.y + 36};
            const std::string body = "The next scroll is hidden inside the Pharaoh`s chariot. Can you uncover How many spokes does each wheel have, and how does this design help?";

            draw_boxed_text(game, font, body, body_pos,
            16,
            {
                body_pos.x, body_pos.y,
                rect.width, rect.height
            },
            spacing, color);

//            DrawRectangleLinesEx(rect, 1, GREEN);
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
}