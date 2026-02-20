#include "question_panel.h"
#include "game.h"

#include <assert.h>
#include <iostream>

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
    QuestionPanel::QuestionPanel()
    {
        // TODO(): A default background
        for (Button& button : buttons)
        {
            button.on_press = on_button_press;
            button.parent = this;
        }

        timer.on_timeout = on_timer_timeout;
        timer.parent = this;
    }

    void QuestionPanel::set_scale(float scale)
    {
        background.scale = scale;
    }

    void QuestionPanel::set_rect(Rectangle rect)
    {
        background.center_x = (int)(rect.x + rect.width / 2);
        background.center_y = (int)(rect.y + rect.height / 2);
        background.w = (int)(rect.width);
        background.h = (int)(rect.height);
    }

    void QuestionPanel::set_background(const AssetManager& asset_m, const std::string& texture_id)
    {
        background.tex = asset_m.get_asset<Texture>(texture_id);
        assert(background.tex);
    }

    void QuestionPanel::set_question(const QuestionData& _data)
    {
        data = _data;
        
        for (int i = 0; i < 4; i++)
        {
            buttons[i].text = data.options[i];
        }
    }

    void QuestionPanel::flip()
    {
        if (!visible) return;

        background.flip();
    }

    void QuestionPanel::update(Game* game) 
    {
        if (!visible)
        {
            background.visible = false;
        }

        timer.update();

        background.visible = true;
        background.update(game);

        if (background.state == PopupImageState::Shown)
        {
            option_rect.width = background.rect.width;
            option_rect.height = background.rect.height / 2.5f;
            option_rect.x = background.rect.x;
            option_rect.y = background.rect.y + (background.rect.height - option_rect.height);

            float button_w = option_rect.width / 2;
            float button_h = option_rect.height / 2;
            const int button_margin = 0;
            buttons[0].rect = {
                option_rect.x + button_margin, option_rect.y + button_margin,
                button_w, button_h
            };

            buttons[1].rect = {
                option_rect.x + button_margin, option_rect.y + option_rect.height - button_h - button_margin,
                button_w, button_h
            };

            buttons[2].rect = {
                option_rect.x + option_rect.width - button_w - button_margin,
                option_rect.y + button_margin,
                button_w, button_h
            };

            buttons[3].rect = {
                option_rect.x + option_rect.width - button_w - button_margin,
                option_rect.y + option_rect.height - button_h - button_margin,
                button_w, button_h
            };
        }

        for (Button& button : buttons)
        {
            button.visible = background.state == PopupImageState::Shown;
            button.update(game);

            button.set_texture(game->asset_m.get_asset<Texture>(ASSETS_PATH"wood.jpg"));
            button.outline = true;
        }
    }

    void QuestionPanel::draw(Game* game, const Font& font) const
    {
        if (!visible) return;

        background.draw(game, font);

        for (const Button& button : buttons)
        {
            button.draw(game, font);
        }

        if (background.state == PopupImageState::Shown)
        {
            Rectangle rect = background.rect;
            Color color;
            color.r = 0x12;
            color.g = 0x11;
            color.b = 0x0f;
            color.a = 0xff;

            const float spacing = 1;

            Vector2 text_pos = {rect.x + 4, rect.y + 4};

            game->QueueTextEx(font, "Mission 1: ", text_pos, 20, spacing, color);

            Vector2 body_pos = {text_pos.x, text_pos.y + 36};

            draw_boxed_text(game, font, data.question, body_pos,
            16,
            {
                body_pos.x, body_pos.y,
                rect.width, rect.height
            },
            spacing, color);
        }
    }

    void QuestionPanel::on_button_press(void* panel, Button* button)
    {
        if (!panel)
        {
            return;
        }
        if (!button)
        {
            return;
        }

        // reset other buttons color changes

        QuestionPanel* p = (QuestionPanel*)panel;

        for (Button& button : p->buttons)
        {
            button.text_color = WHITE;
        }

        if (button->text == p->data.answer)
        {
            std::cout << "Correct" << std::endl;
            button->text_color = GREEN;
        }
        else
        {
            std::cout << "Incorrect" << std::endl;
            button->text_color = RED;
        }

        p->timer.start();
        p->timer.user_data = button;
    }

    void QuestionPanel::on_timer_timeout(void *panel, void* user_data)
    {
        if (!panel) return;
        if (!user_data) return;

        QuestionPanel* p = (QuestionPanel*)panel;
        Button* button = (Button*)user_data;
        button->text_color = WHITE;
    }
}