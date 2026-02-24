#ifndef _QUESTION_PANEL_H
#define _QUESTION_PANEL_H

#include "asset_manager.h"
#include "popup_box.h"
#include "button.h"
#include "timer.h"
#include "question.h"

#include <array>
#include <vector>

struct Game;

namespace UI
{
    struct QuestionPanel
    {
        enum class State
        {
            SHOW_NOTHING,
            SHOW_QUESTION,
            SHOW_FEEDBACK,
            SHOW_FINISHED
        };

        bool visible = false;
        std::vector<QuestionData> questions;

        QuestionPanel();

        // Initializes the default look of the panel (button & background textures, colors, etc)
        void init_default(const Game& game);

        void set_scale(float scale);
        void set_rect(Rectangle rect);
        void set_background(const AssetManager& asset_m, const std::string& texture_id);
        void set_state(State new_state);

        void flip();

        void update(Game* game);
        void draw(Game* game, const Font& font) const; 

        bool _current_question_answered = false;

    private:
        PopupBox background;
        Timer timer;

        State state = State::SHOW_NOTHING;
        State prev_state = State::SHOW_NOTHING;

        std::array<Button, 4> option_buttons;
        Rectangle option_rect;

        Button next_question_button;

        QuestionData* data = nullptr;
        int current_question_index = 0;

        void update_layout();

        void set_question(QuestionData* _data);

        // TODO(omar): should these callbacks be private or public ?
        static void on_option_button_press(void* panel, Button* button);
        static void on_next_button_press(void* panel, Button* button);
        static void on_timer_timeout(void* panel, void* user_data);
    };
}

#endif
