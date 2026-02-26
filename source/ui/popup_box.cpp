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
    PopupBox::PopupBox()
    {
        yes_button.text = "Yes";
        no_button.text = "No";
        
        no_button.parent = this;
        yes_button.parent = this;
        no_button.on_press = on_no_press;
        yes_button.on_press = on_yes_press;
    }

    void PopupBox::update(Game* game)
    {
        if (!visible)
        {
            return;
        }

        if (state == PopupBoxState::Shown && (confirm_box))
        {
            yes_button.visible = true;
            no_button.visible = true;    
        }
        else
        {
            yes_button.visible = false;
            no_button.visible = false;
        }

        switch (state)
        {
            case PopupBoxState::Hidden:
            {
                if (animate_typing)
                {
                    percent_visible = 0;
                }
            } break;

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

            case PopupBoxState::Shown:
            {
                if (animate_typing)
                {
                    if (percent_visible == 1)
                    {
                        if (IsKeyPressed(KEY_ENTER))
                        {
                            percent_visible = 0;
                        }
                    }

                    if (IsKeyPressed(KEY_M))
                    {
                        percent_visible = 1;
                    }

                    sped_up = IsKeyDown(KEY_ENTER);
                    if (percent_visible < 1)
                    {
                        int speed = chars_per_second;
                        if (sped_up)
                        {
                            speed *= 2;
                        }

                        float show_speed = (float)(speed) / (float)(body.length());
                        percent_visible += show_speed * GetFrameTime();
                    }
                    else if (percent_visible > 1)
                    {
                        percent_visible = 1;
                    }
                }
                else
                {
                    percent_visible = 1;
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

        no_button.rect.width = 48;
        no_button.rect.height = 24;
        no_button.rect.x = rect.x + rect.width - yes_button.rect.width;
        no_button.rect.y = rect.y + rect.height - yes_button.rect.height;

        yes_button.rect = no_button.rect;
        yes_button.rect.x -= yes_button.rect.width + 2;

        yes_button.update(game);
        no_button.update(game);
    }

    void PopupBox::draw(Game* game, const Font& font) const
    {
        assert(game);

        if (!visible || (state == PopupBoxState::Hidden))
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

            float shown_body_len = body.length() * percent_visible;
            std::string shown_body = body.substr(0, (size_t)shown_body_len);

            draw_boxed_text(game, font, shown_body, body_pos, body_sz, {
                body_pos.x, body_pos.y,
                rect.width, rect.height
            },
            text_spacing, text_color);
        }

        yes_button.draw(game, font);
        no_button.draw(game, font);
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

    void PopupBox::on_yes_press(void *p, Button *button)
    {
        printf("Yes\n");
    }

    void PopupBox::on_no_press(void *p, Button *button)
    {
        printf("No\n");
    }
}