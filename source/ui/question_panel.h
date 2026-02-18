#ifndef _QUESTION_PANEL_H
#define _QUESTION_PANEL_H

#include "asset_manager.h"
#include "popup_image.h"
#include "button.h"

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

        void flip();

        void update(Game* game);
        void draw(Game* game, const Font& font) const; 

    private:
        PopupImage background;

        std::array<Button, 4> buttons;
        Rectangle option_rect;
//        Button button;
    };
}

#endif