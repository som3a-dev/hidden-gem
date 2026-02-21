#ifndef _QUESTION_PANEL_H
#define _QUESTION_PANEL_H

#include "asset_manager.h"
#include "popup_image.h"
#include "button.h"
#include "timer.h"
#include "question.h"

#include <array>

struct Game;

namespace UI
{
    struct QuestionPanel
    {
        bool visible = false;

        QuestionPanel();

        void set_scale(float scale);
        void set_rect(Rectangle rect);
        void set_background(const AssetManager& asset_m, const std::string& texture_id);
        void set_question(const QuestionData& _data);

        void flip();

        void update(Game* game);
        void draw(Game* game, const Font& font) const; 

    private:
        PopupImage background;
        Timer timer;

        std::array<Button, 4> buttons;
        Rectangle option_rect;

        QuestionData data;

        void set_buttons_layout();

        // TODO(omar): should these callbacks be private or public ?
        static void on_button_press(void* panel, Button* button);
        static void on_timer_timeout(void* panel, void* user_data);
    };
}


#endif