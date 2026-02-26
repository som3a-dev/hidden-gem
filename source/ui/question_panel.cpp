#include "question_panel.h"
#include "game.h"

#include <assert.h>
#include <iostream>
#include <string>

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
        for (Button& button : option_buttons)
        {
            button.set_texture(game.asset_m.get_asset<Texture>(ASSETS_PATH"wood.jpg"));
        }

        next_question_button.set_texture(game.asset_m.get_asset<Texture>(ASSETS_PATH"wood.jpg"));

        background.text_color = BLACK;
        background.bg_color = WHITE;
        background.percent_visible = 1;
        background.animate_typing = false;
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
        background.bg_texture = asset_m.get_asset<Texture>(texture_id);
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

                background.header = "Well done!";
                if (data)
                {
                    background.body = data->feedback;
                }
            } break;

            case State::SHOW_QUESTION:
            {
                update_layout();
                next_question_button.visible = false;
                if (data)
                {
                    background.header = "Mission: ";
                    background.body = data->question;

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

                if (data)
                {
                    background.header = "Nice job!";
                    background.body = "You just finished all the missions!";
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

        if (background.get_state() != PopupBoxState::Shown)
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
    }

    void QuestionPanel::update_layout()
    {
        Rectangle rect = background.get_rect();

        next_question_button.rect.width = 80;
        next_question_button.rect.height = 30;
        next_question_button.rect.x = rect.x + rect.width - next_question_button.rect.width;
        next_question_button.rect.y = rect.y + rect.height - next_question_button.rect.height;

        option_rect.width = rect.width;
        option_rect.height = rect.height / 2.5f;
        option_rect.x = rect.x;
        option_rect.y = rect.y + (rect.height - option_rect.height);

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

            if ((p->parent) && (p->on_question_answered))
            {
                p->on_question_answered(p->parent);
            }
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