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
        for (Button& button : option_buttons)
        {
            button.on_press = on_option_button_press;
            button.parent = this;
        }

        next_question_button.text = "Next";
        next_question_button.parent = this;
        next_question_button.on_press = on_next_button_press;

        timer.on_timeout = on_timer_timeout;
        timer.parent = this;
    }

    void QuestionPanel::init_default(const Game& game)
    {
        // TODO(): A default background
        for (Button& button : option_buttons)
        {
            button.set_texture(game.asset_m.get_asset<Texture>(ASSETS_PATH"wood.jpg"));
        }

        next_question_button.set_texture(game.asset_m.get_asset<Texture>(ASSETS_PATH"wood.jpg"));
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

    void QuestionPanel::set_question(QuestionData* _data)
    {
        assert(_data);
        data = _data;
        
        for (int i = 0; i < data->option_count; i++)
        {
            option_buttons[i].text = data->options[i];
            option_buttons[i].outline = true;
        }

        for (int i = data->option_count; i < QUESTION_MAX_OPTIONS; i++)
        {
            option_buttons[i].visible = false;
        }

        update_layout();
        _current_question_answered = false;
    }

    void QuestionPanel::set_state(State new_state)
    {
        if (state == new_state)
        {
            return;
        }

        timer.stop();
        switch (new_state)
        {
            case State::SHOW_NOTHING:
            {
                for (Button& button : option_buttons)
                {
                    button.visible = false;
                }
                next_question_button.visible = false;
            } break;

            case State::SHOW_FEEDBACK:
            {
                for (Button& button : option_buttons)
                {
                    button.visible = false;
                    button.text_color = WHITE;
                }
                next_question_button.visible = true; 
            } break;

            case State::SHOW_QUESTION:
            {
                update_layout();
                next_question_button.visible = false;

                if (data)
                {
                    for (int i = 0; i < data->option_count; i++)
                    {
                        option_buttons[i].visible = true;
                    }
                }
            } break;

            case State::SHOW_FINISHED:
            {
                next_question_button.visible = false;
                for (Button& button : option_buttons)
                {
                    button.visible = false;
                }
            } break;
        }

        prev_state = state;
        state = new_state;
    }

    void QuestionPanel::flip()
    {
        if (!visible) return;

        background.flip();
    }

    void QuestionPanel::update(Game* game) 
    {
        background.visible = visible;
        if (!visible) return;

        timer.update();
        background.update(game);
        for (Button& button : option_buttons)
        {
            button.update(game);
        }
        next_question_button.update(game);

        printf("%d\n", state);
        if (background.state != PopupImageState::Shown)
        {
            set_state(State::SHOW_NOTHING);
        }
        else
        {
            if ((data == nullptr) && (!questions.empty()))
            {
                set_question(questions.data());
                set_state(State::SHOW_QUESTION);
            }
            else if ((current_question_index >= (questions.size() - 1)) && state != State::SHOW_QUESTION)
            {
                set_state(State::SHOW_FINISHED);
            }
            else if (state == State::SHOW_NOTHING)
            {
                // Panel was closed then opened again, go back to the last state
                set_state(prev_state);
            }
        }
    }

    void QuestionPanel::draw(Game* game, const Font& font) const
    {
        if (!visible) return;

        background.draw(game, font);

        for (const Button& button : option_buttons)
        {
            button.draw(game, font);
        }

        next_question_button.draw(game, font);

        Rectangle rect = background.rect;
        const int header_sz = 20;
        const int body_sz = 16;
        const float text_spacing = 1;
        Vector2 header_pos = {rect.x + 4, rect.y + 4};
        Vector2 body_pos = {header_pos.x, header_pos.y + header_sz * 2};
        Color color;
        color.r = 0x12;
        color.g = 0x11;
        color.b = 0x0f;
        color.a = 0xff;

        switch (state)
        {
            case State::SHOW_QUESTION:
            {
                game->QueueTextEx(font, "Mission: ", header_pos, header_sz, text_spacing, color);

                if (data)
                {
                    draw_boxed_text(game, font, data->question, body_pos,
                    body_sz,
                    {
                        body_pos.x, body_pos.y,
                        rect.width, rect.height
                    },
                    text_spacing, color);
                }
            } break;

            case State::SHOW_FEEDBACK:
            {
                game->QueueTextEx(font, "Well done!", header_pos, header_sz, text_spacing, color);

                if (data)
                {
                    draw_boxed_text(game, font, data->feedback, body_pos,
                    body_sz,
                    {
                        body_pos.x, body_pos.y,
                        rect.width, rect.height
                    },
                    text_spacing, color);
                }
            } break;

            case State::SHOW_FINISHED:
            {
                game->QueueTextEx(font, "Nice job!", header_pos, header_sz, text_spacing, color);
                draw_boxed_text(game, font, "You just finished all the missions!", body_pos,
                body_sz,
                {
                    body_pos.x, body_pos.y,
                    rect.width, rect.height
                },
                text_spacing, color);
            } break;
        }
    }

    void QuestionPanel::update_layout()
    {
        next_question_button.rect.width = 80;
        next_question_button.rect.height = 30;
        next_question_button.rect.x = background.rect.x + background.rect.width - next_question_button.rect.width;
        next_question_button.rect.y = background.rect.y + background.rect.height - next_question_button.rect.height;

        option_rect.width = background.rect.width;
        option_rect.height = background.rect.height / 2.5f;
        option_rect.x = background.rect.x;
        option_rect.y = background.rect.y + (background.rect.height - option_rect.height);

        const int button_margin = 0;

        if (data)
        {
            switch (data->option_count)
            {
                case 4:
                {
                    float button_w = option_rect.width / 2;
                    float button_h = option_rect.height / 2;
                    option_buttons[0].rect = {
                        option_rect.x + button_margin, option_rect.y + button_margin,
                        button_w, button_h
                    };

                    option_buttons[1].rect = {
                        option_rect.x + button_margin, option_rect.y + option_rect.height - button_h - button_margin,
                        button_w, button_h
                    };

                    option_buttons[2].rect = {
                        option_rect.x + option_rect.width - button_w - button_margin,
                        option_rect.y + button_margin,
                        button_w, button_h
                    };

                    option_buttons[3].rect = {
                        option_rect.x + option_rect.width - button_w - button_margin,
                        option_rect.y + option_rect.height - button_h - button_margin,
                        button_w, button_h
                    };
                } break;

                case 3:
                {
                    float button_w = option_rect.width / 2;
                    float button_h = option_rect.height / 2;
                    option_buttons[0].rect = {
                        option_rect.x + button_margin, option_rect.y + button_margin,
                        button_w, button_h
                    };

                    option_buttons[1].rect = {
                        option_rect.x + option_rect.width - button_w - button_margin,
                        option_rect.y + button_margin,
                        button_w, button_h
                    };

                    button_w = option_rect.width;
                    option_buttons[2].rect = {
                        option_rect.x + button_margin,
                        option_rect.y + button_h + button_margin,
                        button_w, button_h
                    };
                } break;
            }
        }
    }

    void QuestionPanel::on_option_button_press(void *panel, Button *button)
    {
        assert(panel);
        assert(button);
        QuestionPanel* p = (QuestionPanel*)panel;

        // reset other option_buttons color changes
        for (Button& button : p->option_buttons)
        {
            button.text_color = WHITE;
        }

        if (button->text == p->data->answer)
        {
            std::cout << "Correct" << std::endl;

            button->text_color = GREEN;
            p->_current_question_answered = true;

/*            if (p->current_question_index < (p->questions.size()-1))
            {
                p->set_state(State::SHOW_FEEDBACK);
            }*/
        }
        else
        {
            std::cout << "Incorrect" << std::endl;

            button->text_color = RED;
        }

        p->timer.start();
        p->timer.user_data = button;
    }

    void QuestionPanel::on_next_button_press(void *panel, Button *button)
    {
        assert(panel);
        assert(button);

        QuestionPanel* p = (QuestionPanel*)panel;
        p->current_question_index++;
        if (p->questions.size() > p->current_question_index)
        {
            p->set_question(p->questions.data() + p->current_question_index);
            p->set_state(State::SHOW_QUESTION);

            printf("Advanced to question %d\n", p->current_question_index);
        }
        else
        {
            printf("Questions are done\n");
        }
    }

    void QuestionPanel::on_timer_timeout(void *panel, void *user_data)
    {
        assert(panel);
        assert(user_data);

        QuestionPanel* p = (QuestionPanel*)panel;
        Button* button = (Button*)user_data;
        button->text_color = WHITE;

        if (p->_current_question_answered)
        {
            if (p->current_question_index >= (p->questions.size() - 1))
            {
                p->set_state(State::SHOW_FINISHED);
            }
            else
            {
                p->set_state(State::SHOW_FEEDBACK);
            }
        }
    }
}