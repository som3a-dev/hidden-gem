#ifndef _QUESTION_PANEL_H
#define _QUESTION_PANEL_H

#include "asset_manager.h"
#include "popup_image.h"

struct Game;

namespace UI
{
    struct QuestionPanel
    {
        bool visible = false;
        PopupImage background;

        QuestionPanel();

        void set_scale(float scale);
        void set_background(const AssetManager& asset_m, const std::string& texture_id);

        void flip();

        void update(float dt);
        void draw(Game* game, const Font& font, const GameDrawBuffer& draw_buf) const;
    };
}

#endif